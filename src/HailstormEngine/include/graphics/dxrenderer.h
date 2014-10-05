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
    ID3D10Device * GetDevice() { return mDevice.Get(); }

    // TODO: THIS IS A TERRIBLE HACK FIGURE OUT HOW TO NOT DO THIS
    void GetProjectionMatrix(D3DXMATRIX *projectionOut) const;

    // Create a font that can be used for drawing text.
    HRESULT CreateRenderFont(
        const std::wstring& fontName,
        int fontSize,
        ID3D10Device * pDevice,
        ID3DX10Font ** ppFontOut) const;

protected:
    virtual void OnStartRenderer() override;
    virtual void OnStopRenderer() override;
    virtual void OnRenderFrame(const DemoScene& scene, TimeT currentTime, TimeT deltaTime) override;
    virtual void OnWindowResized(const Size& screenSize) override;

private:
    HRESULT CreateRenderDevice(ID3D10Device **ppDeviceOut, IDXGISwapChain **ppSwapChainOut) const;
    HRESULT CreateDefaultRasterizerState(ID3D10Device *pDevice, ID3D10RasterizerState **ppRasterStateOut) const;
    HRESULT CreateWireframeRasterizerrState(ID3D10Device *pDevice, ID3D10RasterizerState **ppRasterStateOut) const;
    
    HRESULT CreateDeviceViews(
        ID3D10Device *pDevice,
        ID3D10RenderTargetView **ppRenderTargetViewOut,
        ID3D10Texture2D **ppDepthStencilBufferOut,
        ID3D10DepthStencilView **ppDepthStencilViewOut) const;

    void UpdateDeviceViews(
        ID3D10Device *pDevice,
        ID3D10RenderTargetView *pRenderTargetView,
        ID3D10DepthStencilView *pDepthStencilView,
        const Size& viewportSize);

    void SetViewport(ID3D10Device *pDevice, const Size& viewportSize);

    void ReleaseDeviceViews();

    HRESULT OnStartRenderFrame(TimeT currentTime, TimeT deltaTime);
    HRESULT OnFinishRenderFrame(TimeT currentTime, TimeT deltaTime);

private:
    /// Handle to render window.
    HWND mHwnd;

    /// Flag that is set when frame rendering is underway.
    bool mIsFrameBeingRendered;

    /// Pointer to the main rendering window
    std::shared_ptr<IWindow> mWindow;

    /// Pointer to the D3D 10 device
    Microsoft::WRL::ComPtr<ID3D10Device> mDevice;

    /// Pointer to the renderer swap chain
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;

    /// Pointer to the render target view
    Microsoft::WRL::ComPtr<ID3D10RenderTargetView> mRenderTargetView;

    /// Pointer to the depth stencil texture buffer
    Microsoft::WRL::ComPtr<ID3D10Texture2D> mDepthStencilBuffer;

    /// Pointer to the depth stencil view
    Microsoft::WRL::ComPtr<ID3D10DepthStencilView> mDepthStencilView;

    Microsoft::WRL::ComPtr<ID3D10RasterizerState> mDefaultRasterizerState;
    Microsoft::WRL::ComPtr<ID3D10RasterizerState> mWireframeRasterizerState;

    /// Pointer to the renderer font
    Microsoft::WRL::ComPtr<ID3DX10Font> mRendererFont;

    /// Number of multi samples (Anti aliasing)
    UINT mMultisampleCount;

    /// Multi sample quality
    UINT mMultisampleQuality;

    /// Flag if we are rendering in windowed mode or full screen
    bool mWindowedMode;
	
    /// The currently running graphics content manager
    std::unique_ptr<GraphicsContentManager> mContentManager;
    D3DXMATRIX mProjection;
};

#endif