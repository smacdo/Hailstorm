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
#include "graphics/meshfactory.h"

#include <d3d10_1.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <memory>
#include <string>

#include "graphics/dxutils.h"
#include "graphics/staticmesh.h"
#include "graphics/staticmeshvertex.h"
#include "common/platform_windows.h"
#include "common/platform.h"

/**
 * Mesh factory constructor. Initializes the mesh factory with a pointer to the
 * active Direct3d renderer
 */
MeshFactory::MeshFactory( const std::string& dataDir,
                          ID3D10Device1 * pRenderDevice )
    : mpRenderDevice( pRenderDevice ),
      mpStaticMeshFX( NULL ),
      mpStaticMeshTechnique( NULL ),
      mpStaticMeshInputLayout( NULL )
{
    assert( pRenderDevice != NULL );
    init( dataDir );
}

/**
 * Mesh factory destructor
 */
MeshFactory::~MeshFactory()
{
    SafeRelease( &mpStaticMeshFX );
    SafeRelease( &mpStaticMeshInputLayout );
}

/**
 * Generates a new static mesh for a 3d box
 */
std::shared_ptr<StaticMesh> MeshFactory::createBox( float scale ) const
{
    assert( mpRenderDevice != NULL );

    const unsigned int VERTEX_COUNT = 8;
    const unsigned int FACE_COUNT   = 12;

    // The vertex buffer
    const StaticMeshVertex VERTICES[VERTEX_COUNT] =
    {
        {  D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) },
        {  D3DXVECTOR3( -1.0f, +1.0f, -1.0f ), D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) },
        {  D3DXVECTOR3( +1.0f, +1.0f, -1.0f ), D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f ) },
        {  D3DXVECTOR3( +1.0f, -1.0f, -1.0f ), D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) },
        {  D3DXVECTOR3( -1.0f, -1.0f, +1.0f ), D3DXCOLOR( 0.0f, 1.0f, 1.0f, 1.0f ) },
        {  D3DXVECTOR3( -1.0f, +1.0f, +1.0f ), D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f ) },
        {  D3DXVECTOR3( +1.0f, +1.0f, +1.0f ), D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f ) },
        {  D3DXVECTOR3( +1.0f, -1.0f, +1.0f ), D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) },
    };

    // The index buffer
    const unsigned int INDICES[FACE_COUNT*3] =
    {
        // Front face
        0u, 1u, 2u,
        0u, 2u, 3u,

        // Back face
        4u, 6u, 5u,
        4u, 7u, 6u,

        // Left face
        4u, 5u, 1u,
        4u, 1u, 0u,

        // Right face
        3u, 2u, 6u,
        3u, 6u, 7u,

        // Top face
        1u, 5u, 6u,
        1u, 6u, 2u,

        // Bottom face
        4u, 0u, 3u,
        4u, 3u, 7u
    };

    // Construct and return the static mesh object
    std::vector<unsigned int> indices( &INDICES[0], &INDICES[FACE_COUNT*3] );

    return std::shared_ptr<StaticMesh>(
            new StaticMesh( mpRenderDevice, VERTICES, VERTEX_COUNT, indices ) );
}

/**
 * Initializes the mesh factory. Only call this once, and call it from the
 * class constructor
 */
void MeshFactory::init( const std::string& dataDir )
{
    // Make sure we are initializing only once
    assert( mpStaticMeshTechnique == NULL );
    assert( mpRenderDevice != NULL );

    // Create string that contains file path to effects file
    std::wstring fxfilepath = WinApp::ToWideString(dataDir) +
                              std::wstring( L"\\shaders\\staticmesh.fx" );

    // Load the shader FX file from disk
    ID3D10Blob *pError = NULL;
    HRESULT result     = S_OK;
    DWORD shaderFlags  = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG;

    result = D3DX10CreateEffectFromFile( fxfilepath.c_str(),
                                         NULL,
                                         NULL,
                                         "fx_4_0",
                                         shaderFlags,
                                         NULL,
                                         mpRenderDevice,
                                         NULL,
                                         NULL,
                                         &mpStaticMeshFX,
                                         &pError,
                                         NULL );

    // Check if there were errors while loading the shader from disk
    if ( FAILED(result) )
    {
        if ( pError != NULL )
        {
            // Convert the error blob into a string
            std::string error = reinterpret_cast<const char*>( pError->GetBufferPointer() );

            // Now report the error to the player, and then exit since we can't
            // recover from this
            App::raiseFatalError( "Failed to load static mesh shader", error );
            App::quit( App::EPROGRAM_FATAL_ERROR, "Failed to load static mesh shader" );
        }
        else
        {
            DxUtils::CheckResult( result, true, "Load static mesh shader" );
        }
    }

    // Make sure the effect file was loaded
    assert( mpStaticMeshFX != NULL );

    // Now grab the technique
    mpStaticMeshTechnique = mpStaticMeshFX->GetTechniqueByName( "DefaultStaticMeshTechnique" );
    assert( mpStaticMeshTechnique != NULL );

    // Create the vertex layout expected in a static mesh
    const unsigned int NUM_VERTEX_ELEMENTS = 2;
    D3D10_INPUT_ELEMENT_DESC staticVertexDesc[NUM_VERTEX_ELEMENTS] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }
    };

    // Grab a description struct from the loaded effects file for the first stage.
    // We'll use this to create an input layout
    D3D10_PASS_DESC staticPassDesc;
    assert( mpStaticMeshTechnique->GetPassByIndex( 0 ) != NULL );

    mpStaticMeshTechnique->GetPassByIndex( 0 )->GetDesc( &staticPassDesc );
    
    // Now use the vertex description and the effects pass description to create
    // an input layout we can bind vertices to
    result = mpRenderDevice->CreateInputLayout( staticVertexDesc,
                                                NUM_VERTEX_ELEMENTS,
                                                staticPassDesc.pIAInputSignature,
                                                staticPassDesc.IAInputSignatureSize,
                                                &mpStaticMeshInputLayout );

    DxUtils::CheckResult( result, true, "Creating a static mesh input layout" );
}