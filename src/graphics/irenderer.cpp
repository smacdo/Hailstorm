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
#include "gui/iwindow.h"
#include "common/logging.h"

/**
 * Renderer constructor
 */
IRenderer::IRenderer( IWindow *pWindow )
    : mpWindow( pWindow ),
      mRendererCreatedAndRunning( false )
{
    assert( pWindow != NULL && "The renderer needs a window" );
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
bool IRenderer::initialize()
{
    // Make sure we don't start the renderer more than once
    assert2( mRendererCreatedAndRunning == false, "Cannot start when already running" );

    // Let the renderer perform any needed start up code
    LOG_INFO("Renderer") << "Initializing the renderer";

    if ( onStartRenderer() )
    {
        mRendererCreatedAndRunning = true;
    }

    // Now let the 
    LOG_NOTICE("Renderer") << "The renderer has been initialized";
    return mRendererCreatedAndRunning;
}

/**
 * Ticks the graphics renderer
 */
void IRenderer::tick( TimeT currentTime, TimeT deltaTime )
{
    // Do not do anything if the renderer is not running
    if (! mRendererCreatedAndRunning )
    {
        return;
    }

    // Was the window resized? Make sure to let give the renderer a chance to
    // intercept this event before we start the next frame
    if ( mpWindow->wasResized() && (!mpWindow->isResizing()) && (!mpWindow->isPaused()) )
    {
        LOG_NOTICE("Renderer")
            << "Renderer resized to " << mpWindow->width()
            << " x "                  << mpWindow->height();

        // Raise the onResize event which will let a derived renderer handle this
        // event
        assert( mpWindow->width() > 0 );
        assert( mpWindow->height() > 0 );

        resizeRenderWindow( mpWindow->width(), mpWindow->height() );

        // Clear the resize flag so we do not constantly resize
        mpWindow->clearResizedFlag();
    }

    // Keep rendering whatever is going on
    onRenderFrame( currentTime, deltaTime );
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
        onStopRenderer();

        mRendererCreatedAndRunning = false;
    }

    LOG_NOTICE("Renderer") << "The renderer has been stopped";
}

/**
 * Return a pointer to the renderer's main window
 */
IWindow* IRenderer::renderWindow()
{
    return mpWindow;
}