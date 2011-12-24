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
#ifndef SCOTT_HAILSTORM_GRAPHICS_DXRENDERER_H
#define SCOTT_HAILSTORM_GRAPHICS_DXRENDERER_H

#include "graphics/irenderer.h"

// Forward declarations
class MainWindow;
struct ID3D10Device1;
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
    HRESULT createDeviceAndSwapChain();
    HRESULT createRenderTarget();
    void createViewport();
    bool verifyResult( HRESULT result, const std::string& action ) const;

private:
    MainWindow * mpMainWindow;
    ID3D10Device1 * mpDevice;
    IDXGISwapChain * mpSwapChain;
    ID3D10RenderTargetView * mpRenderTargetView;
};

#endif