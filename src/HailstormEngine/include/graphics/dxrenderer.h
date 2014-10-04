/*
 * Copyright 2011 - 2014 Scott MacDonald
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
#include <d3dx10.h>             // temporary while we host the camera matrices
#include <string>

#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

// Forward declarations
class MainWindow;
class DemoScene;
class GraphicsContentManager;
class LandscapeMesh;
class WaterMesh;

struct IDXGISwapChain;
struct ID3D10RenderTargetView;
struct ID3D10Texture2D;
struct ID3D10DepthStencilView;
struct ID3D10Device;
struct ID3DX10Font;
struct ID3D10Effect;
struct ID3D10EffectTechnique;
struct ID3D10InputLayout;
struct ID3D10EffectMatrixVariable;
struct ID3D10RasterizerState;

/**
 * This is the DirectX implementation of the abstract renderer
 */
class DXRenderer : public IRenderer
{
public:
    explicit DXRenderer(IWindow *pWindow, HWND hwnd);
    virtual ~DXRenderer();

    HRESULT LoadFxFile(const std::wstring& fxFilePath, ID3D10Effect ** ppEffectOut) const;
    void SetDefaultRendering();
    void SetWireframeRendering();

    // DONT STORE THIS POINTER WHEN CALLING
    ID3D10Device * GetDevice() { return mpDevice; }

    // TODO: THIS IS A TERRIBLE HACK FIGURE OUT HOW TO NOT DO THIS
    void GetProjectionMatrix(D3DXMATRIX *projectionOut) const;

    static bool verifyResult(HRESULT result, const std::string& action);

protected:
    virtual bool onStartRenderer() override;
    virtual void onStopRenderer() override;

    
    virtual void OnRenderFrame(const DemoScene& scene, TimeT currentTime, TimeT deltaTime) override;

    void OnStartRenderFrame(TimeT currentTime, TimeT deltaTime);
    void OnFinishRenderFrame(TimeT currentTime, TimeT deltaTime);

    virtual bool resizeRenderWindow( unsigned int width, unsigned int height );

private:
    bool createDeviceViews();
    void releaseDeviceViews();
    bool createRenderDevice();
	bool buildVertexLayout();
    
	void buildRenderStates();
	void buildLights();
    void destroyRenderDevice();
    bool createRenderFont();
    

private:
    /// Handle to render window.
    HWND mHwnd;

    /// Pointer to the main rendering window
    IWindow * mpMainWindow;

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

	ID3D10RasterizerState * mpDefaultRasterizerState;
	ID3D10RasterizerState * mpWireframeRS;

    /// Pointer to the renderer font
    ID3DX10Font * mpRendererFont;

    /// Number of multi samples (Anti aliasing)
    UINT mMultisampleCount;

    /// Multi sample quality
    UINT mMultisampleQuality;

    /// Flag if we are rendering in windowed mode or full screen
    bool mWindowedMode;

	
    /// The currently running graphics content manager
    GraphicsContentManager* mpContentManager;
    D3DXMATRIX mProjection;
};

#endif