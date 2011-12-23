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

#include <d3d10_1.h>
#include <d3d10.h>
#include <d3dx10.h>

DXRenderer::DXRenderer( MainWindow *pWindow )
    : IRenderer( pWindow ),
      mpDevice( NULL ),
      mpSwapChain( NULL ),
      mpRenderTargetView( NULL ),
      mpMainWindow( pWindow )
{
    // empty
}

DXRenderer::~DXRenderer()
{
    SafeRelease( &mpDevice );
    SafeRelease( &mpSwapChain );
    SafeRelease( &mpRenderTargetView );
}

/**
 * Called at the start of the program, and allows the renderer to create itself
 * and then set up all needed configuration and resources
 */
bool DXRenderer::onStartRenderer()
{
    HRESULT result;

    // Create our device and swap chain
    result = createDeviceAndSwapChain();

    if (! verifyResult( result, "Creating DirectX device and swap chain" ) )
    {
        return false;
    }

    // Create the render target view
    result = createRenderTarget();

    if (! verifyResult( result, "Creating render target view") )
    {
        return false;
    }

    // Now set up the viewport
    createViewport();

    // The renderer has been created and initialized properly
    return true;
}

void DXRenderer::onStopRenderer()
{

}

/**
 * Time to draw something
 */
void DXRenderer::onRenderFrame( float currentTime, float deltaTime )
{
    mpDevice->ClearRenderTargetView( mpRenderTargetView,
                                     D3DXCOLOR( 0.f, 0.2f, 0.4f, 1.0f ) );

    mpSwapChain->Present( 0, 0 );
}

/**
 * Configures and then creates the DirectX 10 device and as wellas the DXGI
 * swap chain structure.
 *
 * \return  Result of the action. S_OK if it worked, otherwise something went
 *          wrong
 */
HRESULT DXRenderer::createDeviceAndSwapChain()
{
    HRESULT result = S_OK;

    // Init a struct to hold swap chain description
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory( &scd, sizeof(DXGI_SWAP_CHAIN_DESC) );

    // Fill out the swap chain struct description
    scd.BufferCount       = 1;        // one back buffer
    scd.BufferDesc.Width  = mpMainWindow->width();
    scd.BufferDesc.Height = mpMainWindow->height();
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32 bit color
    scd.BufferDesc.RefreshRate.Numerator   = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow      = mpMainWindow->windowHandle();
    scd.SampleDesc.Count  = 4;         // multisample count
    scd.Windowed          = true;

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

    return result;
}

/**
 * Creates the DirectX backbuffer object, and assigns it to be the render
 * target.
 *
 * \return  An HRESULT representing the success/failure of this action
 */
HRESULT DXRenderer::createRenderTarget()
{
    HRESULT result = S_OK;
    ID3D10Texture2D *pBackBufferTexture = NULL;

    // Get the address of the backbuffer
    result = mpSwapChain->GetBuffer( 0,
                                     __uuidof(ID3D10Texture2D),
                                     reinterpret_cast<LPVOID*>(&pBackBufferTexture) );

    if ( result != S_OK )
    {
        return result;
    }
    else
    {
        assert( pBackBufferTexture != NULL );
    }

    // Now bind the back buffer to the render target view
    result = mpDevice->CreateRenderTargetView( pBackBufferTexture, NULL, &mpRenderTargetView );

    if ( result != S_OK )
    {
        return result;
    }
    else
    {
        assert( mpRenderTargetView != NULL );
    }

    // We no longer need the backbuffer object now that we've bound it to the
    // render target view
    SafeRelease( &pBackBufferTexture );

    // Bind the render target to the output merger state
    mpDevice->OMSetRenderTargets( 1, &mpRenderTargetView, NULL );
    return S_OK;
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

    mpDevice->RSSetViewports( 1, &viewport );
}

/**
 * Takes a DirectX HRESULT return code, and checks that it was succesfull. This
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