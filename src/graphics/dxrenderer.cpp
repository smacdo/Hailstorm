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
#include "graphics/dxrenderer.h"
#include "graphics/dxutils.h"
#include "gui/mainwindow.h"
#include "common/logging.h"

#include <d3d10_1.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <d2d1.h>

#define DXVERIFY(expr,msg) if (!verifyResult(expr,msg)) { return false; }

/**
 * DirectX renderer constructor
 */
DXRenderer::DXRenderer( MainWindow *pWindow )
    : IRenderer( pWindow ),
      mpDDrawFactory( NULL ),
      mpDDrawRenderTarget( NULL ),
      mpDevice( NULL ),
      mpSwapChain( NULL ),
      mpRenderTargetView( NULL ),
      mpMainWindow( pWindow ),
      mpDepthStencilBuffer( NULL ),
      mpDepthStencilView( NULL ),
      mMultisampleCount( 4 ),
      mMultisampleQuality( D3D10_STANDARD_MULTISAMPLE_PATTERN )
{
    // We need to have a valid window handle
    assert( pWindow->windowHandle() != NULL );
}

/**
 * DirectX renderer destructor
 */
DXRenderer::~DXRenderer()
{
    // Unbind the render target view before releasing objects
    if ( mpDevice != NULL )
    {
        mpDevice->OMSetRenderTargets( 0, NULL, NULL );
    }

    SafeRelease( &mpDevice );
    SafeRelease( &mpSwapChain );
    SafeRelease( &mpRenderTargetView );
    SafeRelease( &mpDepthStencilBuffer );
    SafeRelease( &mpRendererFont );
}

/**
 * Called at the start of the program, and allows the renderer to create itself
 * and then set up all needed configuration and resources
 */
bool DXRenderer::onStartRenderer()
{
    // Create our device and swap chain
    if(! createDeviceAndSwapChain() )
    {
        return false;
    }

    // Generate the depth and stencil buffer before creating the render target
    // view
    if (! createDepthAndStencilBuffer() )
    {
        return false;
    }

    // Create the render target view
    if (! createRenderTarget() )
    {
        return false;
    }

    // Now set up the viewport
    createViewport();

    // We also want a renderer font that we can use to draw basic text with
    if (! createRenderFont() )
    {
        return false;
    }

    // The renderer has been created and initialized properly
    return true;
}

void DXRenderer::onStopRenderer()
{

}

/**
 * Handle the rendering window being resized by the player
 */
void DXRenderer::onResizeWindow( unsigned int width, unsigned int height )
{
    LOG_DEBUG("Renderer")
        << "Handling window resize to " << width << " x " << height;

}

/**
 * Time to draw something
 */
void DXRenderer::onRenderFrame( float currentTime, float deltaTime )
{
    // Clear the back buffer and the depth stencil view before doing doing any
    // rendering
    mpDevice->ClearRenderTargetView( mpRenderTargetView,
                                     D3DXCOLOR( 0.f, 0.2f, 0.4f, 1.0f ) );

    mpDevice->ClearDepthStencilView( mpDepthStencilView,
                                     D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL,
                                     1.0f,
                                     0 );

    // Draw some text
    const D3DXCOLOR BLACK( 1.0f, 1.0f, 1.0f, 1.0f );
    RECT R = { 5, 5, 0, 0 };

    mpRendererFont->DrawTextW( 0, L"Hello world", -1, &R, DT_NOCLIP, BLACK );
    mpSwapChain->Present( 0, 0 );
}

/**
 * Configures and then creates the DirectX 10 device and as well as the DXGI
 * swap chain structure.
 *
 * \return  Result of the action. S_OK if it worked, otherwise something went
 *          wrong
 */
bool DXRenderer::createDeviceAndSwapChain()
{
    LOG_DEBUG("Renderer") << "Creating DirectX device and swap chain";
    HRESULT result = S_OK;

    // Init a struct to hold swap chain description
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory( &scd, sizeof(DXGI_SWAP_CHAIN_DESC) );

    // Fill out the swap chain struct description
    scd.BufferCount        = 1;        // one back buffer
    scd.BufferDesc.Width   = mpMainWindow->width();
    scd.BufferDesc.Height  = mpMainWindow->height();
    scd.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM; // 32 bit color
    scd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow       = mpMainWindow->windowHandle();
    scd.SampleDesc.Count   = mMultisampleCount;
    scd.SampleDesc.Quality = mMultisampleQuality;
    scd.Windowed           = true;

    scd.BufferDesc.RefreshRate.Numerator   = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;

    // Set up windows flags that we will be passing to directx
    UINT flags =
        D3D10_CREATE_DEVICE_SINGLETHREADED |    // no need to use threads atm
        D3D10_CREATE_DEVICE_BGRA_SUPPORT   |    // required for direct2d
        D3D10_CREATE_DEVICE_DEBUG;              // support debug layer

    // Create a device, device context and swap chain
    result = D3D10CreateDeviceAndSwapChain1(
        NULL,                           // use default IDXGIAdapter
        D3D10_DRIVER_TYPE_HARDWARE,     // hardware support only
        NULL,                           // no software renderer DLL
        flags,                          // kinda obvious...
        D3D10_FEATURE_LEVEL_10_1,       // want 10.1 support
        D3D10_1_SDK_VERSION,            // want DX 10
        &scd,                           // swap chain desc from above
        &mpSwapChain,                   // address to assign pointer
        &mpDevice );                    // address to assign pointer

    // Did the device creation work?
    return verifyResult( result, "Creating the device and swap chain" );
}

/**
 * Creates the DirectX backbuffer object, and assigns it to be the render
 * target.
 *
 * \return  An HRESULT representing the success/failure of this action
 */
bool DXRenderer::createRenderTarget()
{
    HRESULT result = S_OK;
    ID3D10Texture2D *pBackBufferTexture = NULL;

    // Get the address of the backbuffer
    result = mpSwapChain->GetBuffer( 0,
                                     __uuidof(ID3D10Texture2D),
                                     reinterpret_cast<LPVOID*>(&pBackBufferTexture) );

    if (! verifyResult( result, "Obtaining the backbuffer texture object" ) )
    {
        return false;
    }

    // Now bind the back buffer to the render target view
    LOG_DEBUG("Renderer") << "Creating the render target view";
    result = mpDevice->CreateRenderTargetView( pBackBufferTexture, NULL, &mpRenderTargetView );

    if (! verifyResult( result, "Creating the render target view" ) )
    {
        return false;
    }
    else
    {
        assert( mpRenderTargetView != NULL );
    }

    // We no longer need the backbuffer object now that we've bound it to the
    // render target view
    SafeRelease( &pBackBufferTexture );

    // Bind the render target to the output merger state
    LOG_DEBUG("Renderer") << "Binding the render target to the output merger stage";
    mpDevice->OMSetRenderTargets( 1, &mpRenderTargetView, mpDepthStencilView );
    return true;
}

/**
 * Creates a simple system font that the renderer can use to display simple
 * text on the screen
 */
bool DXRenderer::createRenderFont()
{
    LOG_DEBUG("Renderer") << "Creating render font";

    D3DX10_FONT_DESC font;
    ZeroMemory( &font, sizeof(D3DX10_FONT_DESC) );

    // Describe the font that we wish to create
    font.Height = 24;
    font.MipLevels = 1;
    font.CharSet = DEFAULT_CHARSET;
    font.OutputPrecision = OUT_DEFAULT_PRECIS;
    font.Quality = DEFAULT_QUALITY;
    font.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

    wcscpy( font.FaceName, L"Courier New" );

    // Now create that font
    HRESULT result = D3DX10CreateFontIndirect( mpDevice, &font, &mpRendererFont );
    return verifyResult( result, "Creating a render font" );
}

/**
 * Creates the depth and stencil buffer
 */
bool DXRenderer::createDepthAndStencilBuffer()
{
    D3D10_TEXTURE2D_DESC depthStencilDesc;
    HRESULT result = S_OK;

    ZeroMemory( &depthStencilDesc, sizeof(D3D10_TEXTURE2D_DESC) );

    // Depth and stencil buffer texture description
    depthStencilDesc.Width              = mpMainWindow->width();
    depthStencilDesc.Height             = mpMainWindow->height();
    depthStencilDesc.MipLevels          = 1;
    depthStencilDesc.ArraySize          = 1;
    depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = mMultisampleCount;
    depthStencilDesc.SampleDesc.Quality = mMultisampleQuality;
    depthStencilDesc.Usage              = D3D10_USAGE_DEFAULT;
    depthStencilDesc.BindFlags          = D3D10_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags     = 0;
    depthStencilDesc.MiscFlags          = 0;

    // Create the depth and stencil buffer texture
    result = mpDevice->CreateTexture2D( &depthStencilDesc, 0, &mpDepthStencilBuffer );

    DXVERIFY( result, "Creating depth stencil buffer" );
    assert( mpDepthStencilBuffer != NULL );

    // Now create the depth and stencil view
    result = mpDevice->CreateDepthStencilView( mpDepthStencilBuffer, 0, &mpDepthStencilView );

    DXVERIFY( result, "Creating depth stencil view" );
    assert( mpDepthStencilView != NULL );

    return true;
}

/**
 * Support the viewport 
 */
void DXRenderer::createViewport()
{ 
    D3D10_VIEWPORT viewport;
    ZeroMemory( &viewport, sizeof(D3D10_VIEWPORT) );

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width    = mpMainWindow->width();
    viewport.Height   = mpMainWindow->height();

    LOG_DEBUG("Renderer") << "Creating the display viewport";
    mpDevice->RSSetViewports( 1, &viewport );
}

/**
 * Starts up DirectDraw, and creates a hardware rendering surface that can be
 * used later on to perform DirectDraw commands
 */
bool DXRenderer::startDirectDraw()
{
    LOG_DEBUG("Renderer") << "Initializing DirectDraw support";
    ID3D10Texture2D *pBackBufferTexture = NULL;
    HRESULT result = S_OK;

    return true;
}

/**
 * Takes a DirectX HRESULT return code, and checks that it was successfully. This
 * method will create an error message that includes diagnostic information if
 * the value is not HRESULT, and then return false.
 *
 * \param  result  The HRESULT to check
 * \param  action  The action that the renderer was attempting to perform
 * \return         True if the check succeeded, false otherwise
 */
bool DXRenderer::verifyResult( HRESULT result, const std::string& action ) const
{
    // Don't do anything if it worked
    if ( result == S_OK )
    {
        return true;
    }

    // Now figure out what the error status code is
    std::string errorText;

    switch ( result )
    {
        case D3D10_ERROR_FILE_NOT_FOUND:
            errorText = "File not found";
            break;

        case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
            errorText = "Too many unique state objects";
            break;

        case D3DERR_INVALIDCALL:
            errorText = "Invalid call (is a parameter wrong or null?)";
            break;

        case D3DERR_WASSTILLDRAWING:
            errorText = "Previous blit is still transferring information";
            break;

        case E_FAIL:
            errorText = "Cannot create device w/ debug layer when not installed";
            break;

        case E_INVALIDARG:
            errorText = "An invalid argument was passed";
            break;

        case E_OUTOFMEMORY:
            errorText = "D3D could not allocate sufficient memory";
            break;

        case S_FALSE:
            errorText = "S_FALSE? WTF?";
            break;

        default:
            errorText = "NO_ERROR_TEXT_AVAILABLE";
            break;
    }

    // Raise the error with the application before returning false
    App::raiseError( "Failed to perform: " + action,
                     errorText );
    return false;
}