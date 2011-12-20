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
#include "graphics/dxrenderer.h"
#include "graphics/dxutils.h"
#include "gui/mainwindow.h"

#include <d3d10.h>
#include <d3dx10.h>

DXRenderer::DXRenderer( MainWindow *pWindow )
    : IRenderer( pWindow ),
      mpDevice( NULL ),
      mpSwapChain( NULL ),
      mpBackBuffer( NULL )
{
    // empty
}

DXRenderer::~DXRenderer()
{
    SafeRelease( &mpDevice );
    SafeRelease( &mpSwapChain );
    SafeRelease( &mpBackBuffer );
}

bool DXRenderer::onStartRenderer()
{
    return true;
}

void DXRenderer::onStopRenderer()
{

}

void DXRenderer::onRenderFrame( float currentTime, float deltaTime )
{

}