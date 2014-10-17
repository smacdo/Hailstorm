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

#include "bases/Initializable.h"
#include "runtime/gametime.h"
#include <string>

#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

// Forward declarations
class RenderingWindow;
class DemoScene;
class GraphicsContentManager;
class LandscapeMesh;
class WaterMesh;
class Camera;
struct Size;

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
 * DirectX renderer.
 * TODO: Describe class better.
 *
 * TODO: Remove HWND and DXGI creation from this class. Calling code should generate these values and pass it in to the
 *       Initialization method. This will make it easier to add Windows store app support.
 */
class DXRenderer : public Initializable
{
public:
    DXRenderer(
        std::shared_ptr<Camera> camera,
        std::shared_ptr<RenderingWindow> window);
    virtual ~DXRenderer();

    void Initialize();

    // TODO: Don't pass scene here. Add a method for DXRenderer called "AttachScene". Update should call
    //       Scene::Update() and Scene::Render() at the appropriate time.
    void Update(const DemoScene& scene, TimeT currentTime, TimeT deltaTime);

    HRESULT LoadFxFile(const std::wstring& fxFilePath, ID3D10Effect ** ppEffectOut) const;
    void SetDefaultRendering();
    void SetWireframeRendering();

    // DONT STORE THIS POINTER WHEN CALLING
    ID3D10Device * GetDevice() { return mDevice.Get(); }

    // Create a font that can be used for drawing text.
    HRESULT CreateRenderFont(
        const std::wstring& fontName,
        int fontSize,
        ID3D10Device * pDevice,
        ID3DX10Font ** ppFontOut) const;

protected:
    HRESULT StartRenderingFrame(TimeT currentTime, TimeT deltaTime);
    HRESULT RenderFrame(const DemoScene& scene, TimeT currentTime, TimeT deltaTime);
    void FinishRenderingFrame(TimeT currentTime, TimeT deltaTime);

    void OnWindowResized(const Size& screenSize);

private:
    HRESULT CreateRenderDevice(HWND hwnd, ID3D10Device **ppDeviceOut, IDXGISwapChain **ppSwapChainOut) const;
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

private:
    /// Main rendering window.
    std::shared_ptr<RenderingWindow> mWindow;

    /// Player camera.
    std::shared_ptr<Camera> mCamera;

    /// Flag that is set when frame rendering is underway.
    bool mIsFrameBeingRendered;

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
};

#endif