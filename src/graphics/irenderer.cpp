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

IRenderer::IRenderer( IWindow *pWindow )
    : mpWindow( pWindow ),
      mRendererCreatedAndRunning( false )
{
    assert( pWindow != NULL && "The renderer needs a window" );
}

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
    if ( onStartRenderer() )
    {
        mRendererCreatedAndRunning = true;
    }

    // Now let the 
    return mRendererCreatedAndRunning;
}

/**
 * Ticks the graphics renderer
 */
void IRenderer::tick()
{
    // Do not do anything if the renderer is not running
    if (! mRendererCreatedAndRunning )
    {
        return;
    }

    onRenderFrame( 0.0f, 0.0f );
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
        onStopRenderer();
        mRendererCreatedAndRunning = false;
    }
}

/**
 * Return a pointer to the renderer's main window
 */
IWindow* IRenderer::renderWindow()
{
    return mpWindow;
}