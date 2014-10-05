/*
 * Copyright 2011 Scott MacDonald
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "stdafx.h"
#include "graphics/irenderer.h"
#include "graphics/DemoScene.h"
#include "gui/iwindow.h"
#include "runtime/logging.h"
#include "runtime/Size.h"

/**
 * Renderer constructor
 */
IRenderer::IRenderer(std::shared_ptr<IWindow> window)
    : mWindow(window),
      mRendererCreatedAndRunning( false )
{
    AssertNotNull(window.get());
}

/**
 * Renderer destructor
 */
IRenderer::~IRenderer()
{
    // nothing
}

/**
 * Starts the renderer up by initializing the graphics driver, creating any
 * needed data structures and then returning
 *
 * \return  True if the renderer started up properly, false otherwise
 */
void IRenderer::initialize()
{
    // Let the renderer perform any needed start up code
    LOG_INFO("Renderer") << "Initializing the renderer";
    mRendererCreatedAndRunning = true;
    OnStartRenderer();
}

/**
 * Ticks the graphics renderer
 */
void IRenderer::Update(const DemoScene& scene, TimeT currentTime, TimeT deltaTime)
{
    // Do not do anything if the renderer is not running
    if (! mRendererCreatedAndRunning )
    {
        return;
    }

    // Was the window resized? Make sure to let give the renderer a chance to intercept this event before we start the
    // next frame.
    std::shared_ptr<IWindow> renderWindow = GetWindow();

    if (renderWindow->wasResized() && (!renderWindow->isResizing()) && (!renderWindow->isPaused()))
    {
        LOG_NOTICE("Renderer")
            << "Renderer resized to " << renderWindow->width()
            << " x " << renderWindow->height();

        // Raise the onResize event which will let a derived renderer handle this
        // event
        assert(renderWindow->width() > 0);
        assert(renderWindow->height() > 0);

        OnWindowResized(Size{ renderWindow->width(), renderWindow->height() });

        // Clear the resize flag so we do not constantly resize
        renderWindow->clearResizedFlag();
    }

    // Keep rendering whatever is going on
    OnRenderFrame(scene, currentTime, deltaTime);
}

/**
 * Shuts the renderer down and releases any hardware resources that were created
 * by this renderer
 */
void IRenderer::stop()
{
    // Only attempt to stop if the renderer is actually... you know... running :p
    if ( mRendererCreatedAndRunning )
    {
        LOG_INFO("Renderer") << "Stopping the renderer";
        OnStopRenderer();

        mRendererCreatedAndRunning = false;
    }

    LOG_NOTICE("Renderer") << "The renderer has been stopped";
}