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
#ifndef SCOTT_HAILSTORM_GRAPHICS_DXRENDERER_H
#define SCOTT_HAILSTORM_GRAPHICS_DXRENDERER_H

#include "graphics/irenderer.h"

// Forward declarations
class MainWindow;
struct ID3D10Device;
struct IDXGISwapChain;
struct ID3D10RenderTargetView;

/**
 * This is the DirectX implementation of the abstract renderer
 */
class DXRenderer : public IRenderer
{
public:
    explicit DXRenderer( MainWindow *pWindow );
    virtual ~DXRenderer();

protected:
    virtual bool onStartRenderer();
    virtual void onStopRenderer();
    virtual void onRenderFrame( float currentTime, float deltaTime );

private:
    ID3D10Device * mpDevice;
    IDXGISwapChain * mpSwapChain;
    ID3D10RenderTargetView * mpBackBuffer;
};

#endif