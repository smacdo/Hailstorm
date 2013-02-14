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
#include "graphics/landscapemesh.h"

#include <DXGI.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <vector>

#include "graphics/dxrenderer.h"
#include "graphics/dxutils.h"

const int CUBE_VERTEX_COUNT = 8;
const int CUBE_FACE_COUNT = 12;

/**
 * The vertex structure that is used in the static mesh
 */
struct LandscapeVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXCOLOR   diffuse;
	D3DXCOLOR   spec; // (r, g, b, specPower);
};

namespace
{
	/**
     * Returns an appropriate color for the landscape height.
     */
	void FindVertexColor( float y, D3DXCOLOR * pDiffuse, D3DXCOLOR * pSpecular )
	{
		if ( y < -10.0f )
		{
			*pDiffuse  = D3DXCOLOR( 1.0f, 0.96f, 0.62f, 1.0f );
			*pSpecular = D3DXCOLOR( 0.2f, 0.2f, 0.2f, 32.0f );
		}
		else if ( y < 5.0f )
		{
			*pDiffuse  = D3DXCOLOR( 0.48f, 0.77f, 0.46f, 1.0f );
			*pSpecular = D3DXCOLOR( 0.2f, 0.2f, 0.2f, 32.0f );
		}
		else if ( y < 12.0f )
		{
			*pDiffuse  = D3DXCOLOR( 0.1f, 0.48f, 0.19f, 1.0f );
			*pSpecular = D3DXCOLOR( 0.2f, 0.2f, 0.2f, 32.0f );
		}
		else if ( y < 20.0f )
		{
			*pDiffuse  = D3DXCOLOR( 0.45f, 0.39f, 0.34f, 1.0f );
			*pSpecular = D3DXCOLOR( 0.4f, 0.4f, 0.4f, 64.0f );
		}
		else
		{
			*pDiffuse  = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
			*pSpecular = D3DXCOLOR( 0.8f, 0.8f, 0.8f, 64.0f );
		}		
	}
}

/**
 * Static mesh constructor that takes an already constructed vertex and index
 * buffer.
 */
LandscapeMesh::LandscapeMesh( ID3D10Device * pRenderDevice, unsigned int rows, unsigned int cols )
    : mNumRows( rows ),
	  mNumCols( cols ),
	  mVertexCount( 0 ),
      mFaceCount( 0 ),
      mpVertexBuffer( NULL ),
      mpIndexBuffer( NULL )
{
	init( pRenderDevice );
}

/**
 * Static mesh destructor
 */
LandscapeMesh::~LandscapeMesh()
{
    SafeRelease( &mpVertexBuffer );
    SafeRelease( &mpIndexBuffer );
}

/**
 * Magic.
 */
float LandscapeMesh::getHeight( float x, float z ) const
{
	return 0.3f * ( z * sinf( 0.1f * x ) + x * cosf( 0.1f * z ) );
}

/**
 * Takes an array of vertices and indices, uploads them to the video hardware
 * and places their data buffers in mVertexbuffer/mIndexBuffer
 */
void LandscapeMesh::init( ID3D10Device * pRenderDevice )
{
	const float dx = 1.0f;

	// Initialize a vertex buffer that contains all the vertices making up our cube
	mVertexCount = mNumRows * mNumCols;
	mFaceCount = ( mNumRows - 1 ) * ( mNumCols - 1 ) * 2;

	// Create a mesh that models the landscape
	std::vector<LandscapeVertex> vertices( mVertexCount );
	
	float halfWidth  = ( mNumCols - 1 ) * dx * 0.5f;
	float halfDepth  = ( mNumRows - 1 ) * dx * 0.5f;

	for ( unsigned int i = 0; i < mNumRows; ++i )
	{
		float z = halfDepth - i * dx;

		for ( unsigned int j = 0; j < mNumCols; ++j )
		{
			unsigned int index = i * mNumCols + j;
			float x = -halfWidth + j * dx;

			// Graph of this function looks like a mountain range.
			float y = getHeight( x, z );

			vertices[index].pos = D3DXVECTOR3( x, y, z );
			FindVertexColor( y, &vertices[index].diffuse, &vertices[index].spec );

			// Calculate normal for this point. Since this landscape is mathematically generated we can use
			// the book's nice formula for finding a smooth normal;
			//  FORMULA: n = ( -df / dx, 1, -df/dz)
			D3DXVECTOR3 normal;

			normal.x = -0.03f * z * cosf( 0.1f * x ) - 0.3f * cosf( 0.1f * z );
			normal.y =  1.0f;
			normal.z = -0.3f * sinf( 0.1f * x ) + 0.03f * x * sinf( 0.1f * z );

			D3DXVec3Normalize( &vertices[index].normal, &normal );
		}
	}

    // Describe the layout of the vertex buffer and create it.
    D3D10_BUFFER_DESC vbd;
    ZeroMemory( &vbd, sizeof(D3D10_BUFFER_DESC) );
    
    vbd.Usage          = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth      = sizeof(LandscapeVertex) * mVertexCount;
    vbd.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags      = 0;
    
    D3D10_SUBRESOURCE_DATA vInitData;
    ZeroMemory( &vInitData, sizeof(D3D10_SUBRESOURCE_DATA) );

    vInitData.pSysMem = &vertices[0];

	// Upload the vertex buffer to the graphics card.
	HRESULT result = pRenderDevice->CreateBuffer( &vbd, &vInitData, &mpVertexBuffer );

	if (! DxUtils::CheckResult( result, true, "Creating the landscape vertex buffer" ) )
	{
		return;
	}

	// Generate the landscape index buffer
	std::vector<DWORD> indices( mFaceCount * 3 );
	int k = 0;

	for ( unsigned int i = 0; i < mNumRows - 1; ++i )
	{
		for ( unsigned int j = 0; j < mNumCols - 1; ++j )
		{
			indices[k]   = i * mNumCols + j;
			indices[k+1] = i * mNumCols + j + 1;
			indices[k+2] = ( i + 1 ) * mNumCols + j;

			indices[k+3] = ( i + 1 ) * mNumCols + j;
			indices[k+4] = i * mNumCols + j + 1;
			indices[k+5] = ( i +1 ) * mNumCols + j + 1;

			k += 6; // next quad
		}
	}

    // Describe the layout of the index buffer and create it.
    D3D10_BUFFER_DESC ibd;
    ZeroMemory( &ibd, sizeof(D3D10_BUFFER_DESC) );

    ibd.Usage     = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mFaceCount * 3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    
    D3D10_SUBRESOURCE_DATA iInitData;
    ZeroMemory( &iInitData, sizeof(D3D10_SUBRESOURCE_DATA) );

    iInitData.pSysMem = &indices[0];

	// Upload the index buffer to the graphics card.
    result = pRenderDevice->CreateBuffer( &ibd, &iInitData, &mpIndexBuffer );

    if (! DxUtils::CheckResult( result, true, "Creating the landscape index buffer" ) )
    {
        return;
    }
}

/**
 * Render the cube
 */
void LandscapeMesh::draw( ID3D10Device * pDevice ) const
{
    assert( pDevice != NULL );

    const unsigned int stride = sizeof( LandscapeVertex );
    const unsigned int offset = 0;

    if ( mFaceCount > 0 )
    {
        // Need to cast away const-ness when calling DirectX... /sigh
        ID3D10Buffer * pVertexBuffer = const_cast<ID3D10Buffer*>( mpVertexBuffer );
        ID3D10Buffer * pIndexBuffer  = const_cast<ID3D10Buffer*>( mpIndexBuffer  );

        pDevice->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );
        pDevice->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
        pDevice->DrawIndexed( mFaceCount * 3, 0, 0 );
    }
}