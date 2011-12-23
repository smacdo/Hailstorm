/*
 * Copyright (C) 2011 Scott MacDonald. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "stdafx.h"
#include "graphics/irenderer.h"

IRenderer::IRenderer( Window *pWindow )
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
Window* IRenderer::renderWindow()
{
    return mpWindow;
}