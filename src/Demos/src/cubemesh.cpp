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
#include "cubemesh.h"
#include "runtime/debugging.h"

#include <DXGI.h>
#include <d3d10.h>
#include <d3dx10.h>

#include "graphics/dxrenderer.h"
#include "graphics/DirectXExceptions.h"

#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

const int CUBE_VERTEX_COUNT = 8;
const int CUBE_FACE_COUNT = 12;

/**
 * The vertex structure that is used in the static mesh
 */
struct CubeMeshVertex
{
    D3DXVECTOR3 pos;
    D3DXCOLOR color;
};

/**
 * Static mesh constructor that takes an already constructed vertex and index
 * buffer.
 */
CubeMesh::CubeMesh(ID3D10Device * pRenderDevice)
    : mVertexCount(0),
      mFaceCount(0),
      mVertexBuffer(),
      mIndexBuffer()
{
	Init( pRenderDevice );
}

/**
 * Static mesh destructor
 */
CubeMesh::~CubeMesh()
{
}

/**
 * Takes an array of vertices and indices, uploads them to the video hardware
 * and places their data buffers in mVertexbuffer/mIndexBuffer
 */
void CubeMesh::Init(ID3D10Device * pRenderDevice)
{
	// Initialize a vertex buffer that contains all the vertices making up our cube
	mVertexCount = 8;
	mFaceCount = 12;

	const CubeMeshVertex VERTICES[ CUBE_VERTEX_COUNT ] =
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
	const unsigned int INDICES[ CUBE_FACE_COUNT * 3 ] =
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

    // Describe the layout of the vertex buffer and create it.
    D3D10_BUFFER_DESC vbd;
    ZeroMemory( &vbd, sizeof(D3D10_BUFFER_DESC) );
    
    vbd.Usage          = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth      = sizeof(CubeMeshVertex) * mVertexCount;
    vbd.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags      = 0;
    
    D3D10_SUBRESOURCE_DATA vInitData;
    ZeroMemory( &vInitData, sizeof(D3D10_SUBRESOURCE_DATA) );

    vInitData.pSysMem = &VERTICES[0];

	// Upload the vertex buffer to the graphics card.
	HRESULT hr = pRenderDevice->CreateBuffer( &vbd, &vInitData, &mVertexBuffer );
    
    if (SUCCEEDED(hr))
    {
        // Describe the layout of the index buffer and create it.
        D3D10_BUFFER_DESC ibd;
        ZeroMemory(&ibd, sizeof(D3D10_BUFFER_DESC));

        ibd.Usage = D3D10_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof(DWORD) * mFaceCount * 3;
        ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;

        D3D10_SUBRESOURCE_DATA iInitData;
        ZeroMemory(&iInitData, sizeof(D3D10_SUBRESOURCE_DATA));

        iInitData.pSysMem = &INDICES[0];

        // Upload the index buffer to the graphics card.
        hr = pRenderDevice->CreateBuffer(&ibd, &iInitData, &mIndexBuffer);
    }

    if (FAILED(hr))
    {
        throw new DirectXException(hr, L"Initializing cube mesh", L"", __FILE__, __LINE__);
    }
}

/**
 * Render the cube
 */
void CubeMesh::Draw( ID3D10Device * pDevice ) const
{
    assert( pDevice != NULL );

    const unsigned int stride = sizeof( CubeMeshVertex );
    const unsigned int offset = 0;

    if ( mFaceCount > 0 )
    {
        // Need to cast away const-ness when calling DirectX... /sigh
        ID3D10Buffer * pVertexBuffer = const_cast<ID3D10Buffer*>(mVertexBuffer.Get());
        ID3D10Buffer * pIndexBuffer  = const_cast<ID3D10Buffer*>(mIndexBuffer.Get());

        pDevice->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );
        pDevice->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
        pDevice->DrawIndexed( mFaceCount * 3, 0, 0 );
    }
}