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
class GraphicsContentManager;
struct IDXGISwapChain;
struct ID3D10RenderTargetView;
struct ID3D10Texture2D;
struct ID3D10DepthStencilView;
struct ID3D10Device;
struct ID3DX10Font;

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
    virtual bool resizeRenderWindow( unsigned int width, unsigned int height );

private:
    bool createDeviceViews();
    void releaseDeviceViews();
    bool createRenderDevice();
    void destroyRenderDevice();
    bool createRenderFont();
    bool startDirectDraw();
    static bool verifyResult( HRESULT result, const std::string& action );

private:
    /// Pointer to the main rendering window
    MainWindow * mpMainWindow;

    /// Pointer to the D3D 10 device
    ID3D10Device * mpDevice;

    /// Pointer to the renderer swap chain
    IDXGISwapChain * mpSwapChain;

    /// Pointer to the render target view
    ID3D10RenderTargetView * mpRenderTargetView;

    /// Pointer to the depth stencil texture buffer
    ID3D10Texture2D * mpDepthStencilBuffer;

    /// Pointer to the depth stencil view
    ID3D10DepthStencilView * mpDepthStencilView;

    /// Pointer to the renderer font
    ID3DX10Font * mpRendererFont;

    /// Number of multi samples (Anti aliasing)
    UINT mMultisampleCount;

    /// Multi sample quality
    UINT mMultisampleQuality;

    /// Flag if we are rendering in windowed mode or full screen
    bool mWindowedMode;

    /// The currently running graphics content manager
    GraphicsContentManager * mpContentManager;
};

#endif