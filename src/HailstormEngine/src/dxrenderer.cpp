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
#include "stdafx.h"
#include "graphics/dxrenderer.h"
#include "graphics/graphicscontentmanager.h"
#include "gui/iwindow.h"
#include "runtime/logging.h"
#include "runtime/Size.h"
#include "graphics/DirectXExceptions.h"
#include "graphics/DemoScene.h"

#include <DXGI.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

#define DXVERIFY(expr,msg) if (!verifyResult(expr,msg)) { return false; }

/**
 * DirectX renderer constructor
 */
DXRenderer::DXRenderer(IWindow *pWindow, HWND hwnd)
    : IRenderer( pWindow ),
      mHwnd(hwnd),
      mIsFrameBeingRendered(false),
	  mWindow(pWindow),
      mDevice(),
      mSwapChain(),
      mRenderTargetView(),
      mDepthStencilBuffer(),
      mDepthStencilView(),
	  mDefaultRasterizerState(),
      mWireframeRasterizerState(),
      mMultisampleCount(4),
      mMultisampleQuality(1),
      mWindowedMode(true),
      mContentManager()  // this is initialized later
{
    assert(mHwnd != nullptr);    // We need to have a valid window handle
    D3DXMatrixIdentity(&mProjection);
}

/**
 * DirectX renderer destructor
 */
DXRenderer::~DXRenderer()
{
    // Release the device view before destroying the view itself
    ReleaseDeviceViews();
}

/**
 * Called at the start of the program, and allows the renderer to create itself
 * and then set up all needed configuration and resources
 */
void DXRenderer::OnStartRenderer()
{
    // Create our device and swap chain
    HRESULT hr = CreateRenderDevice(&mDevice, &mSwapChain);

    // Create the back buffer and the depth stencil buffer object
    if (SUCCEEDED(hr))
    {
        hr = CreateDeviceViews(
            mDevice.Get(),
            &mRenderTargetView,
            &mDepthStencilBuffer,
            &mDepthStencilView);

        if (SUCCEEDED(hr))
        {
            UpdateDeviceViews(
                mDevice.Get(),
                mRenderTargetView.Get(),
                mDepthStencilView.Get(),
                Size{ mWindow->width(), mWindow->height() });
        }
    }

    // Create rasterizer states.
    if (SUCCEEDED(hr))
    {
        hr = CreateDefaultRasterizerState(mDevice.Get(), &mDefaultRasterizerState);
    }

    if (SUCCEEDED(hr))
    {
        hr = CreateWireframeRasterizerrState(mDevice.Get(), &mWireframeRasterizerState);
    }

    // Create a renderer font to draw with.
    if (SUCCEEDED(hr))
    {
        hr = CreateRenderFont(L"Courier New", 24, mDevice.Get(), &mRendererFont);
    }

    if (SUCCEEDED(hr))
    {
        // Content manager allows us to create and load graphics
        mContentManager.reset(new GraphicsContentManager(mDevice.Get(), L"..\\data"));
    }

    // Check for errors and throw an exception if one happened. We can push propogation of HRESULTs up to the caller,
    // but really there's no hope at this point of recovering if DirectX freaked out.
    if (FAILED(hr))
    {
        throw new DirectXException(hr, L"Window was resized", L"", __FILE__, __LINE__);
    }
}


/**
 * Creates the Direct3D render device and DXGI swap chain.
 */
HRESULT DXRenderer::CreateRenderDevice(ID3D10Device **ppDeviceOut, IDXGISwapChain **ppSwapChainOut) const
{
    LOG_DEBUG("Renderer") << "Creating DirectX device and swap chain";
    VerifyNotNull(ppDeviceOut);
    VerifyNotNull(ppSwapChainOut);

    *ppDeviceOut = nullptr;
    *ppSwapChainOut = nullptr;

    // Describe the swap chain we want to create.
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;                 // One back buffer plz.
    scd.BufferDesc.Width = mWindow->width();
    scd.BufferDesc.Height = mWindow->height();
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32 bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = mHwnd;
    scd.SampleDesc.Count = 1;                 // TODO: mMultisampleCount;
    scd.SampleDesc.Quality = 0;                 // TODO: mMultisampleQuality;
    scd.Windowed = true;

    scd.BufferDesc.RefreshRate.Numerator = 60;    // TODO: Fill this out programatically.
    scd.BufferDesc.RefreshRate.Denominator = 1;

    // Set up windows flags that we will be passing to directx
    UINT flags =
        D3D10_CREATE_DEVICE_SINGLETHREADED	    // No need to use threads at the moment.
#if defined(DEBUG) || defined(_DEBUG)  
        | D3D10_CREATE_DEVICE_DEBUG;            // Enable DirectX debug layer.
#endif

    // Create a device, device context and swap chain
    Microsoft::WRL::ComPtr<ID3D10Device> device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

    HRESULT hr = D3D10CreateDeviceAndSwapChain(
        NULL,                           // use default IDXGIAdapter
        D3D10_DRIVER_TYPE_HARDWARE,     // hardware support only
        NULL,                           // no software renderer DLL
        flags,                          // kinda obvious...
        D3D10_SDK_VERSION,              // want DX 10
        &scd,                           // swap chain desc from above
        &swapChain,                     // Swap chain out
        &device);                       // Device out

    if (SUCCEEDED(hr))
    {
        *ppDeviceOut = device.Detach();
        *ppSwapChainOut = swapChain.Detach();
    }

    return hr;
}

/**
 * This method is responsible for creating the back buffer (render target) and
 * depth stencil buffers. CreateDeviceViews will then bind these buffers to the
 * device, create the window viewport and then return.
 *
 * This should get called after the d3d render device is created, and anytime after
 * the d3d render device has been reset.
 */
HRESULT DXRenderer::CreateDeviceViews(
    ID3D10Device *pDevice,
    ID3D10RenderTargetView **ppRenderTargetViewOut,
    ID3D10Texture2D **ppDepthStencilBufferOut,
    ID3D10DepthStencilView **ppDepthStencilViewOut) const
{
    LOG_DEBUG("Renderer") << "Creating the render target view";

    VerifyNotNull(pDevice);
    VerifyNotNull(ppRenderTargetViewOut);
    VerifyNotNull(ppDepthStencilBufferOut);
    VerifyNotNull(ppDepthStencilViewOut);

    *ppRenderTargetViewOut = nullptr;
    *ppDepthStencilBufferOut = nullptr;
    *ppDepthStencilViewOut = nullptr;

    // Grab a pointer to the texture object that is acting as our main backbuffer.
    Microsoft::WRL::ComPtr<ID3D10Texture2D> backBufferTexture;
    HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), &backBufferTexture);

    // Bind the back buffer to the render target view.
    Microsoft::WRL::ComPtr<ID3D10RenderTargetView> renderTargetView;

    if (SUCCEEDED(hr))
    {
        hr = mDevice->CreateRenderTargetView(backBufferTexture.Get(), nullptr, &renderTargetView);
    }

    // Create a texture that will become the depth and stencil buffer.
    Microsoft::WRL::ComPtr<ID3D10Texture2D> depthStencilBuffer;

    if (SUCCEEDED(hr))
    {
        D3D10_TEXTURE2D_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D10_TEXTURE2D_DESC));

        depthStencilDesc.Width = mWindow->width();
        depthStencilDesc.Height = mWindow->height();
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count = 1; // mMultisampleCount;
        depthStencilDesc.SampleDesc.Quality = 0; // mMultisampleQuality;
        depthStencilDesc.Usage = D3D10_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;

        hr = mDevice->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer);
    }

    // With the depth and stencil buffer texture created, the next step is to create a depth stencil view with that
    // buffer texture.
    Microsoft::WRL::ComPtr<ID3D10DepthStencilView> depthStencilView;

    if (SUCCEEDED(hr))
    {
        LOG_DEBUG("Renderer") << "Creating the depth stencil view";
        hr = mDevice->CreateDepthStencilView(depthStencilBuffer.Get(), 0, &depthStencilView);
    }

    // If all the above operations have succeeded, transfer ownership of the created COM objects to the caller.
    if (SUCCEEDED(hr))
    {
        *ppRenderTargetViewOut = renderTargetView.Detach();
        *ppDepthStencilBufferOut = depthStencilBuffer.Detach();
        *ppDepthStencilViewOut = depthStencilView.Detach();
    }

    return hr;
}

/**
 * This method is responsible for creating the back buffer (render target) and
 * depth stencil buffers. CreateDeviceViews will then bind these buffers to the
 * device, create the window viewport and then return.
 *
 * This should get called after the d3d render device is created, and anytime after
 * the d3d render device has been reset.
 */
void DXRenderer::UpdateDeviceViews(
    ID3D10Device *pDevice,
    ID3D10RenderTargetView *pRenderTargetView,
    ID3D10DepthStencilView *pDepthStencilView,
    const Size& viewportSize)
{
    VerifyNotNull(pDevice);
    VerifyNotNull(pRenderTargetView);
    VerifyNotNull(pDepthStencilView);

    // Bind the render target to the output merger state.
    LOG_DEBUG("Renderer") << "Binding the render target and depth stencil views";

    ID3D10RenderTargetView * renderTargetViews[1] = { pRenderTargetView };
    pDevice->OMSetRenderTargets(1, renderTargetViews, pDepthStencilView);

    // Create the render window viewport, and make it the same size as our rendering window
    SetViewport(pDevice, viewportSize);
}

/**
 * Create solid, textured rasterizer state.
 */
HRESULT DXRenderer::CreateDefaultRasterizerState(
    ID3D10Device *pDevice,
    ID3D10RasterizerState **ppRasterStateOut) const
{
    VerifyNotNull(pDevice);
    VerifyNotNull(ppRasterStateOut);

    *ppRasterStateOut = nullptr;

    D3D10_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D10_RASTERIZER_DESC));

    rasterDesc.CullMode = D3D10_CULL_FRONT;
    rasterDesc.FillMode = D3D10_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = true;
    rasterDesc.DepthBias = false;
    rasterDesc.DepthBiasClamp = 0;
    rasterDesc.SlopeScaledDepthBias = 0;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.ScissorEnable = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.AntialiasedLineEnable = true;

    return mDevice->CreateRasterizerState(&rasterDesc, ppRasterStateOut);
}

/**
* Create wireframe rasterizer state.
*/
HRESULT DXRenderer::CreateWireframeRasterizerrState(
    ID3D10Device *pDevice,
    ID3D10RasterizerState **ppRasterStateOut) const
{
    VerifyNotNull(pDevice);
    VerifyNotNull(ppRasterStateOut);

    *ppRasterStateOut = nullptr;

    D3D10_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D10_RASTERIZER_DESC));

    rasterDesc.FillMode = D3D10_FILL_WIREFRAME;
    rasterDesc.CullMode = D3D10_CULL_BACK;

    return mDevice->CreateRasterizerState(&rasterDesc, ppRasterStateOut);
}

/**
 * Creates a simple system font that the renderer can use to display simple
 * text on the screen
 */
HRESULT DXRenderer::CreateRenderFont(
    const std::wstring& fontName,
    int fontSize,
    ID3D10Device * pDevice,
    ID3DX10Font ** ppFontOut) const
{
    LOG_DEBUG("Renderer") << "Creating render font";
    VerifyNotNull(pDevice);
    VerifyNotNull(ppFontOut);

    *ppFontOut = nullptr;

    // Describe the font that we wish to create
    D3DX10_FONT_DESC fontDesc;
    ZeroMemory(&fontDesc, sizeof(D3DX10_FONT_DESC));

    fontDesc.Height = fontSize;
    fontDesc.MipLevels = 1;
    fontDesc.CharSet = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

    wcscpy_s(fontDesc.FaceName, fontName.c_str());

    // Create the font.
    Microsoft::WRL::ComPtr<ID3DX10Font> fontObject;
    HRESULT hr = D3DX10CreateFontIndirectW(pDevice, &fontDesc, &fontObject);

    if (SUCCEEDED(hr))
    {
        *ppFontOut = fontObject.Detach();
    }

    return hr;
}

void DXRenderer::SetViewport(ID3D10Device *pDevice, const Size& viewportSize)
{
    VerifyNotNull(pDevice);

    // Create the render window viewport, and make it the same size as our rendering window
    D3D10_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D10_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = viewportSize.width;
    viewport.Height = viewportSize.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    LOG_DEBUG("Renderer") << "Setting display viewport";
    pDevice->RSSetViewports(1, &viewport);
}

/**
 * Called when the renderer stops.
 */
void DXRenderer::OnStopRenderer()
{
}

/**
 * Handle the rendering window being resized by the player
 */
void DXRenderer::OnWindowResized(const Size& screenSize)
{
    // Before we can resize the render window we must release the old device views as they hold references to the
    // buffers we shall be destroying
    ReleaseDeviceViews();

    // Resize the swap chain. This will destroy the current back buffer
    HRESULT hr = mSwapChain->ResizeBuffers(
        1,
        screenSize.width,
        screenSize.height,
        DXGI_FORMAT_R8G8B8A8_UNORM,             // TODO: Track this display format so we are consistent.
        0);

    if (SUCCEEDED(hr))
    {
        // Now that we've resized the back buffer, we can proceed with recreating our destroyed device view.
        hr = CreateDeviceViews(
            mDevice.Get(),
            &mRenderTargetView,
            &mDepthStencilBuffer,
            &mDepthStencilView);

        if (SUCCEEDED(hr))
        {
            // Set the viewport dimensions, and update our device views with this new size.
            UpdateDeviceViews(
                mDevice.Get(),
                mRenderTargetView.Get(),
                mDepthStencilView.Get(),
                Size{ mWindow->width(), mWindow->height() });

            // Reset our aspect ratio and the perspective matrix
            //  TODO: Clean this up, use constants and explain what is going on.
            float aspect = static_cast<float>(screenSize.width) / static_cast<float>(screenSize.height);
            D3DXMatrixPerspectiveFovLH(&mProjection, 0.25f * 3.1415927f, aspect, 1.0f, 1000.0f);
        }

    }

    // If we failed, there's no point in sending the error code back to our caller. We screwed up, it's over. Time
    // for the program to face the music...
    if (FAILED(hr))
    {
        throw new DirectXException(hr, L"Window was resized", L"", __FILE__, __LINE__);
    }
}

HRESULT DXRenderer::OnStartRenderFrame(TimeT currentTime, TimeT deltaTime)
{
    // Clear the back buffer and the depth stencil view before doing doing any
    // rendering
    mDevice->ClearRenderTargetView(
        mRenderTargetView.Get(),
        D3DXCOLOR(0.f, 0.2f, 0.4f, 1.0f));

    mDevice->ClearDepthStencilView(
        mDepthStencilView.Get(),
        D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL,
        1.0f,
        0);

    // Restore the renderer's default states, input layout and primitive topology because mFont->DrawText chagnse them.
    // Note that we can restore the default states by passing null.
    float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    mDevice->OMSetDepthStencilState(0, 0);
    mDevice->OMSetBlendState(0, blendFactors, 0xffffffff);

    return S_OK;
}

/**
 * Called when the renderer should produce a frame.
 */
void DXRenderer::OnRenderFrame(const DemoScene& scene, TimeT currentTime, TimeT deltaTime)
{
    mIsFrameBeingRendered = true;
    OnStartRenderFrame(currentTime, deltaTime);

    scene.Render(*this, currentTime, deltaTime);

    // Draw some text
    const D3DXCOLOR BLACK(1.0f, 1.0f, 1.0f, 1.0f);
    RECT R = { 5, 5, 0, 0 };

    mRendererFont->DrawTextW(0, L"Hello world", -1, &R, DT_NOCLIP, BLACK);

    OnFinishRenderFrame(currentTime, deltaTime);   
    mIsFrameBeingRendered = false;
}

/**
* Called when the renderer is finished producing the frame.
*/
HRESULT DXRenderer::OnFinishRenderFrame(TimeT currentTime, TimeT deltaTime)
{
    return mSwapChain->Present(0, 0);      // TODO: Enable vsync.
}

/**
 * Releases the active backbuffer and depth stencil view that are bound to the current d3d device. This should be done
 * in preparation to resizing or resetting the d3d renderer.
 */
void DXRenderer::ReleaseDeviceViews()
{
    LOG_DEBUG("Renderer") << "Releasing the device views (backbuffer, depth+stencil)";

    // Make DirectX happy by binding null views before we go ahead and release everything
    mDevice->OMSetRenderTargets(0, nullptr, nullptr);

    // Release our views and the stencil buffer
    mRenderTargetView.Reset();
    mDepthStencilView.Reset();
    mDepthStencilBuffer.Reset();
}

/**
 * Loads a .FX file from disk.
 */
HRESULT DXRenderer::LoadFxFile(
    const std::wstring& fxFilePath,
    ID3D10Effect ** ppEffectOut) const
{
    VerifyNotNull(ppEffectOut);
    *ppEffectOut = nullptr;

	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    Microsoft::WRL::ComPtr<ID3D10Blob> compilationErrors;
    Microsoft::WRL::ComPtr<ID3D10Effect> loadedEffect;

	HRESULT hr = D3DX10CreateEffectFromFile(
        fxFilePath.c_str(),     // Path to the FX file we want to load.
		nullptr,                // Pointer to an array of shader macros.
		nullptr,                // Pointer to an include interface for compiling.
		"fx_4_0",               // HLSL profile to use when compiling.
		shaderFlags,            // HLSL compiler flags.
		0,                      // No options while compiling the shader.
		mDevice.Get(),          // D3D device that will use this effect.
		nullptr,                // Pointer to an effect pool for sharing variables between threads.
		nullptr,                // Address to a thread pump interface. Not needed.            
        &loadedEffect,          // Load FX object into this guy.
        &compilationErrors,     // Blob holding error string, if there was one.
		nullptr);               // Pointer to out HRESULT. Not neeeded. (Ever?)

    if (SUCCEEDED(hr))
    {
        *ppEffectOut = loadedEffect.Detach();
    }
    else
    {
        if (compilationErrors)      // TODO: Report file, line exception.
        {
            throw ShaderCompileFailedException(hr, fxFilePath, (const char*) compilationErrors->GetBufferPointer());
        }
        else
        {
            throw DirectXException(hr, L"Failed to create effect from FX file", fxFilePath);
        }
    }

	return hr;
}

void DXRenderer::SetDefaultRendering()
{
    mDevice->RSSetState(mDefaultRasterizerState.Get());
}

void DXRenderer::SetWireframeRendering()
{
    mDevice->RSSetState(mWireframeRasterizerState.Get());
}

void DXRenderer::GetProjectionMatrix(D3DXMATRIX *projectionOut) const
{
    VerifyNotNull(projectionOut);
    *projectionOut = mProjection;
}