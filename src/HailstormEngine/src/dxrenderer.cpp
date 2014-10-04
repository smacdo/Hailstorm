/*
 * Copyright 2011-2014 Scott MacDonald
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
#include "graphics/graphicscontentmanager.h"
#include "landscapemesh.h"                  // TODO: Don't use this file directly
#include "watermesh.h"                      // TODO: Don't use this file directly.
#include "gui/iwindow.h"
#include "runtime/logging.h"
#include "runtime/delete.h"
#include "runtime/mathutils.h"

#include <DXGI.h>
#include <d3d10.h>
#include <d3dx10.h>

#define DXVERIFY(expr,msg) if (!verifyResult(expr,msg)) { return false; }

/**
 * DirectX renderer constructor
 */
DXRenderer::DXRenderer(IWindow *pWindow, HWND hwnd)
    : IRenderer( pWindow ),
      mHwnd(hwnd),
	  mpMainWindow( pWindow ),
      mpDevice( NULL ),
      mpSwapChain( NULL ),
      mpRenderTargetView( NULL ),
      mpDepthStencilBuffer( NULL ),
      mpDepthStencilView( NULL ),
	  mpFX( NULL ),
	  mpTechnique( NULL ),
	  mpVertexLayout( NULL ),
	  mpWVP( NULL ),
	  mpWireframeRS( NULL ),
      mMultisampleCount( 4 ),
      mMultisampleQuality( 1 ),
      mWindowedMode( true ),
	  mEyePos( 0.0f, 0.0f, 0.0f ),
	  mRadius( 75 ),
	  mLightType( 0 ),
      mpContentManager( NULL ), // this is initialized later
	  mpCubeMesh( NULL ),
	  mpWaterMesh( NULL )
{
    assert(mHwnd != nullptr);    // We need to have a valid window handle

	D3DXMatrixIdentity( &mView );
	D3DXMatrixIdentity( &mProjection );
	D3DXMatrixIdentity( &mWVP );

	D3DXMatrixIdentity( &mLandTransform );
	D3DXMatrixIdentity( &mWaterTransform );
}

/**
 * DirectX renderer destructor
 */
DXRenderer::~DXRenderer()
{
    // Kill the content manager first
    Delete( mpContentManager );
	Delete( mpCubeMesh );

    // Release the device view before destroying the view itself
    releaseDeviceViews();

	SafeRelease( &mpFX );
	SafeRelease( &mpVertexLayout );
	SafeRelease( &mpWireframeRS );

    SafeRelease( &mpSwapChain );

    // Now destroy the d3d device
    SafeRelease( &mpDevice );

    // Finally make sure we release the renderer font
    SafeRelease( &mpRendererFont );
}

/**
 * Called at the start of the program, and allows the renderer to create itself
 * and then set up all needed configuration and resources
 */
bool DXRenderer::onStartRenderer()
{
    // Create our device and swap chain
    if(! createRenderDevice() )
    {
        return false;
    }

    // Create the back buffer and the depth stencil buffer object
    if (! createDeviceViews() )
    {
        return false;
    }

    // We also want a renderer font that we can use to draw basic text with
    if (! createRenderFont() )
    {
        return false;
    }

	if (! buildFX() | !buildVertexLayout() )
	{
		return false;
	}

	buildRenderStates();
	buildLights();

    // Content manager allows us to create and load graphics
    mpContentManager = new GraphicsContentManager( mpDevice, "..\\data" );
	mpCubeMesh = new LandscapeMesh( mpDevice, 129, 129, 1.0f );
	mpWaterMesh = new WaterMesh( mpDevice, 257, 257, 0.5f, 0.03f, 3.25f, 0.4f );

    // The renderer has been created and initialized properly
    return true;
}

void DXRenderer::onStopRenderer()
{

}

/**
 * Handle the rendering window being resized by the player
 */
bool DXRenderer::resizeRenderWindow( unsigned int width, unsigned int height )
{
    HRESULT result = S_OK;

    // Before we can resize the render window we must release the old device views
    // as they hold references to the buffers we shall be destroying
    releaseDeviceViews();

    // Resize the swap chain. This will destroy the current back buffer
    result = mpSwapChain->ResizeBuffers( 1,
                                         width,
                                         height,
                                         DXGI_FORMAT_R8G8B8A8_UNORM,
                                         0 );

    DXVERIFY( result, "Resizing the swap chain" );

	// Now that we've resized the back buffer, we can proceed with recreating
	// our destroyed device view
	createDeviceViews();

	// Reset our aspect ratio and the perspective matrix
	float aspect = (float) width / (float) height;

	D3DXMatrixPerspectiveFovLH( &mProjection, 0.25f * 3.1415927f, aspect, 1.0f, 1000.0f );

	return true;
}

/**
 * Update animations and whatnot
 */
void DXRenderer::onUpdate( double currentTime, double deltaTime )
{
	// Set up the light type based on user input
	if ( GetKeyState( '1' & 0x8000 ) ) { mLightType = 0; LOG_DEBUG("Renderer") << "Switched to light type 0"; }
	if ( GetKeyState( '2' & 0x8000 ) ) { mLightType = 1; LOG_DEBUG("Renderer") << "Switched to light type 1"; }
	if ( GetKeyState( '3' & 0x8000 ) ) { mLightType = 2; LOG_DEBUG("Renderer") << "Switched to light type 2"; }

	// Every quarter second, generate a random wave
	static float t_base = 0.0f;

	if ( currentTime - t_base >= 0.25f )
	{
		t_base += 0.25f;

		unsigned int i = 5 + rand() % 250;
		unsigned int j = 5 + rand() % 250;

		float r = randF( 1.0f, 2.0f );

		mpWaterMesh->perturb( i, j, r ) ;
	}

	// Make sure the water mesh is kept up to date with ripple animations
	mpWaterMesh->update( (float) deltaTime );

	// Rotate camera around the landscape
	mEyePos.x = mRadius * cosf( static_cast<float>( 0.5 * currentTime ) );
	mEyePos.z = mRadius * sinf( static_cast<float>( 0.5 * currentTime ) );
	mEyePos.y = 50.0f * sinf( static_cast<float>( 0.5 * currentTime ) ) + 75.0f;

	// Rebuild view matrix with the new camera coordinates
	D3DXVECTOR3 target( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );

	D3DXMatrixLookAtLH( &mView, &mEyePos, &target, &up );


	// The point light circles the scene as a function of time, staying seven units above the land's
	// or water's surface.
	mLights[1].pos.x = 50.0f * cosf( (float) currentTime );
	mLights[1].pos.z = 50.0f * sinf( (float) currentTime );
	mLights[1].pos.y = 7.0f + std::max( mpCubeMesh->getHeight( mLights[1].pos.x, mLights[1].pos.z ), 0.0f );

	// The spotlight takes on the camera position and is aimed in the same direction as the camera is
	// looking. In this way it looks like we are holding a flashlight.
	mLights[2].pos = mEyePos;
	D3DXVec3Normalize( &mLights[2].dir, &(target - mEyePos) );
}

/**
 * Time to draw something
 */
void DXRenderer::onRenderFrame()
{
    // Clear the back buffer and the depth stencil view before doing doing any
    // rendering
    mpDevice->ClearRenderTargetView( mpRenderTargetView,
                                     D3DXCOLOR( 0.f, 0.2f, 0.4f, 1.0f ) );

    mpDevice->ClearDepthStencilView( mpDepthStencilView,
                                     D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL,
                                     1.0f,
                                     0 );

	// Restore the renderer's default states, input layout and primitive topology because
	// mFont->DrawText chagnse them. Note that we can restore the default states by passing null.
	mpDevice->OMSetDepthStencilState( 0, 0 );

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	mpDevice->OMSetBlendState( 0, blendFactors, 0xffffffff );
	mpDevice->IASetInputLayout( mpVertexLayout );
	mpDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set per frame constants
	mpFxEyePosVar->SetRawValue( &mEyePos, 0, sizeof( D3DXVECTOR3 ) );
	mpFxLightVar->SetRawValue( &mLights[mLightType], 0, sizeof( Light ) );
	mpFxLightType->SetInt( mLightType );

	// Load the effect technique for cube
	D3D10_TECHNIQUE_DESC technique;
	mpTechnique->GetDesc( &technique );

	// Draw everything
	for ( unsigned int passIndex = 0; passIndex < technique.Passes; ++passIndex )
	{
		ID3D10EffectPass * pPass = mpTechnique->GetPassByIndex( passIndex );
		mpDevice->RSSetState( mpDefaultRasterizerState );

		// Draw the landscape mesh first
		mWVP =  mLandTransform * mView * mProjection;

		mpWVP->SetMatrix( (float*) &mWVP );
		mpWorldVar->SetMatrix( (float*) &mLandTransform );

		pPass->Apply( 0 );
		mpCubeMesh->draw( mpDevice );

		// Draw the water mesh
		mWVP = mWaterTransform * mView * mProjection;
		
		mpWVP->SetMatrix( (float*) &mWVP );
		mpWorldVar->SetMatrix( (float*) &mWaterTransform );

//		mpDevice->RSSetState( mpWireframeRS );
		pPass->Apply( 0 );
		
		mpWaterMesh->draw( mpDevice );
	}

    // Draw some text
    const D3DXCOLOR BLACK( 1.0f, 1.0f, 1.0f, 1.0f );
    RECT R = { 5, 5, 0, 0 };

    mpRendererFont->DrawTextW( 0, L"Hello world", -1, &R, DT_NOCLIP, BLACK );
    mpSwapChain->Present( 0, 0 );
}

/**
 * Creates the Direct3D render device and DXGI swap chain.
 *
 * \return  True if the call succeeded, false otherwise
 */
bool DXRenderer::createRenderDevice()
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
    scd.OutputWindow       = mHwnd;
    scd.SampleDesc.Count   = 1; // mMultisampleCount;
    scd.SampleDesc.Quality = 0; // mMultisampleQuality;
    scd.Windowed           = true;

    scd.BufferDesc.RefreshRate.Numerator   = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;

    // Set up windows flags that we will be passing to directx
    UINT flags =
        D3D10_CREATE_DEVICE_SINGLETHREADED	    // no need to use threads atm
#if defined(DEBUG) || defined(_DEBUG)  
        | D3D10_CREATE_DEVICE_DEBUG;            // support debug layer
#endif

    // Create a device, device context and swap chain
    result = D3D10CreateDeviceAndSwapChain(
        NULL,                           // use default IDXGIAdapter
        D3D10_DRIVER_TYPE_HARDWARE,     // hardware support only
        NULL,                           // no software renderer DLL
        flags,                          // kinda obvious...
        D3D10_SDK_VERSION,              // want DX 10
        &scd,                           // swap chain desc from above
        &mpSwapChain,                   // address to assign pointer
        &mpDevice );                    // address to assign pointer

    // Did the device creation work?
    return verifyResult( result, "Creating the device and swap chain" );
}

/**
 * This method is responsible for creating the back buffer (render target) and
 * depth stencil buffers. CreateDeviceViews will then bind these buffers to the
 * device, create the window viewport and then return.
 *
 * This should get called after the d3d render device is created, and anytime after
 * the d3d render device has been reset.
 *
 * \return  True if the call bound the back buffer and depth stencil view, false
 *          otherwise
 */
bool DXRenderer::createDeviceViews()
{
    LOG_DEBUG("Renderer") << "Creating the render target view";

    HRESULT result = S_OK;
    ID3D10Texture2D *pBackBufferTexture = NULL;

    // Grab a pointer to the texture object that is acting as our main backbuffer
    result = mpSwapChain->GetBuffer( 0,
                                     __uuidof(ID3D10Texture2D),
                                     reinterpret_cast<LPVOID*>(&pBackBufferTexture) );

    DXVERIFY( result, "Obtaining the back buffer texture object" );
    assert( pBackBufferTexture != NULL );

    // Now bind the back buffer to the render target view
    result = mpDevice->CreateRenderTargetView( pBackBufferTexture, NULL, &mpRenderTargetView );

    DXVERIFY( result, "Creating the render target view" );
    assert( mpRenderTargetView != NULL );

	// We no longer need the backbuffer object now that we've bound it to the
	// render target view
	SafeRelease( &pBackBufferTexture );

    // Create the depth and stencil buffer texture
    D3D10_TEXTURE2D_DESC depthStencilDesc;
    ZeroMemory( &depthStencilDesc, sizeof(D3D10_TEXTURE2D_DESC) );

    depthStencilDesc.Width              = mpMainWindow->width();
    depthStencilDesc.Height             = mpMainWindow->height();
    depthStencilDesc.MipLevels          = 1;
    depthStencilDesc.ArraySize          = 1;
    depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = 1; // mMultisampleCount;
    depthStencilDesc.SampleDesc.Quality = 0; // mMultisampleQuality;
    depthStencilDesc.Usage              = D3D10_USAGE_DEFAULT;
    depthStencilDesc.BindFlags          = D3D10_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags     = 0;
    depthStencilDesc.MiscFlags          = 0;

    // Create a texture that will become the depth and stencil buffer
    result = mpDevice->CreateTexture2D( &depthStencilDesc, 0, &mpDepthStencilBuffer );

    DXVERIFY( result, "Creating depth stencil buffer" );
    assert( mpDepthStencilBuffer != NULL );

    // With the depth and stencil buffer texture created, the next step is to
    // create a depth stencil view with that buffer texture.
    LOG_DEBUG("Renderer") << "Creating the depth stencil view";
    result = mpDevice->CreateDepthStencilView( mpDepthStencilBuffer, 0, &mpDepthStencilView );

    DXVERIFY( result, "Creating depth stencil view" );
    assert( mpDepthStencilView != NULL );

    // Bind the render target to the output merger state
    LOG_DEBUG("Renderer") << "Binding the render target and depth stencil views";
    mpDevice->OMSetRenderTargets( 1, &mpRenderTargetView, mpDepthStencilView );

    // Create the render window viewport, and make it the same size as our rendering
    // window
    D3D10_VIEWPORT viewport;
    ZeroMemory( &viewport, sizeof(D3D10_VIEWPORT) );

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width    = mpMainWindow->width();
    viewport.Height   = mpMainWindow->height();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

    LOG_DEBUG("Renderer") << "Creating the display viewport";
    mpDevice->RSSetViewports( 1, &viewport );

    return true;
}

/**
 * Releases the active backbuffer and depth stencil view that are bound to the
 * current d3d device. This should be done in preparation to resizing or resetting
 * the d3d renderer.
 */
void DXRenderer::releaseDeviceViews()
{
    LOG_DEBUG("Renderer")
        << "Releasing the device views (backbuffer, depth+stencil)";

    // Make DirectX happy by binding null views before we go ahead and release
    // everything
    mpDevice->OMSetRenderTargets( 0, NULL, NULL );

    // Release our views and the stencil buffer
    SafeRelease( &mpRenderTargetView );
    SafeRelease( &mpDepthStencilView );
    SafeRelease( &mpDepthStencilBuffer );
}

/**
 * Creates and sets up the vertex buffer.
 */
bool DXRenderer::buildVertexLayout()
{
	// Describe the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Load the default pass from the .fx file we loaded earlier
	D3D10_PASS_DESC passDescription;
	mpTechnique->GetPassByIndex( 0 )->GetDesc( &passDescription );

	// Create the vertex input layout.
	HRESULT result = mpDevice->CreateInputLayout( vertexDescription,
		                                          4,
												  passDescription.pIAInputSignature,
												  passDescription.IAInputSignatureSize,
												  &mpVertexLayout );
	// Make sure it worked
	verifyResult( result, "Creating the vertex input layout" );

	LOG_DEBUG("Renderer") << "Created the vertex input layout.";
	return true;
}

/**
 * Creates the .fx stuff
 */
bool DXRenderer::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob * pCompilationErrors = NULL;
	HRESULT result = D3DX10CreateEffectFromFile(
		L"../data/shaders/landscape.fx",
		0,
		0,
		"fx_4_0",
		shaderFlags,
		0,
		mpDevice,
		0,
		0,
		&mpFX,
		&pCompilationErrors,
		0 );

	if ( FAILED( result ) )
	{
		if ( pCompilationErrors != NULL )
		{
            // TODO: Make this an exception.
//			App::raiseFatalError( (char*) pCompilationErrors->GetBufferPointer(), "Failed to compile cube shader" );
			SafeRelease( &pCompilationErrors );

			return false;
		}
	}

	mpTechnique = mpFX->GetTechniqueByName( "LandscapeTechnique" );

	mpWVP         = mpFX->GetVariableByName( "gWVP" )->AsMatrix();
	mpWorldVar    = mpFX->GetVariableByName( "gWorld" )->AsMatrix();
	mpFxEyePosVar = mpFX->GetVariableByName( "gEyePosW" );
	mpFxLightVar  = mpFX->GetVariableByName( "gLight" );
	mpFxLightType = mpFX->GetVariableByName( "gLightType" )->AsScalar();

	LOG_DEBUG("Renderer") << "Created effects data";
	return true;
}

/**
 * Generate render states
 */
void DXRenderer::buildRenderStates()
{
	// Create the default rasterizer state
	D3D10_RASTERIZER_DESC rasterizerDescription;
	ZeroMemory( &rasterizerDescription, sizeof( D3D10_RASTERIZER_DESC ) );

	rasterizerDescription.CullMode = D3D10_CULL_FRONT;
	rasterizerDescription.FillMode = D3D10_FILL_SOLID;
	rasterizerDescription.FrontCounterClockwise = true;
	rasterizerDescription.DepthBias = false;
	rasterizerDescription.DepthBiasClamp = 0;
	rasterizerDescription.SlopeScaledDepthBias = 0;
	rasterizerDescription.DepthClipEnable = true;
	rasterizerDescription.ScissorEnable = false;
	rasterizerDescription.MultisampleEnable = false;
	rasterizerDescription.AntialiasedLineEnable = true;

	DxUtils::CheckResult( mpDevice->CreateRasterizerState( &rasterizerDescription, &mpDefaultRasterizerState ), true, "Creating default rasterizer state" );

	// Create a transparent rasterizer state
	ZeroMemory( &rasterizerDescription, sizeof( D3D10_RASTERIZER_DESC ) );

	rasterizerDescription.FillMode = D3D10_FILL_WIREFRAME;
	rasterizerDescription.CullMode = D3D10_CULL_BACK;

	DxUtils::CheckResult( mpDevice->CreateRasterizerState( &rasterizerDescription, &mpWireframeRS ), true, "Creating wireframe rasterizer state" );
}

/**
 * Populate light structs
 */
void DXRenderer::buildLights()
{
	// Parallel light
	mLights[0].dir = D3DXVECTOR3( 0.57735f, -0.57735f, 0.57735f );
	mLights[0].ambient = D3DXCOLOR( 0.2f, 0.2f, 0.2f, 1.0f );
	mLights[0].diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	mLights[0].specular = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 1.0f );

	// Point light (position is changed every frame)
	mLights[1].ambient = D3DXCOLOR( 0.4f, 0.4f, 0.4f, 1.0f );
	mLights[1].diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	mLights[1].specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	mLights[1].att.x = 0.0f;
	mLights[1].att.y = 0.1f;
	mLights[1].att.z = 0.0f;
	mLights[1].range = 50.0f;

	// Spotlight -- position and direction changed every frame
	mLights[2].ambient = D3DXCOLOR( 0.4f, 0.4f, 0.4f, 1.0f );
	mLights[2].diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	mLights[2].specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	mLights[2].att.x = 1.0f;
	mLights[2].att.y = 0.0f;
	mLights[2].att.z = 0.0f;
	mLights[2].spotPow = 64.0f;
	mLights[2].range = 10000.0f;
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

    wcscpy_s( font.FaceName, L"Courier New" );

    // Now create that font
    HRESULT result = D3DX10CreateFontIndirect( mpDevice, &font, &mpRendererFont );
    return verifyResult( result, "Creating a render font" );
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
bool DXRenderer::verifyResult( HRESULT result, const std::string& action )
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
    // TODO: Make this an exception. Actually just rewrite this.
    //App::raiseError( "Failed to perform: " + action, errorText );
    return false;
}