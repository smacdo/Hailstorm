/*
 * Copyright 2013 Scott MacDonald
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
#include "graphics/watermesh.h"

#include <DXGI.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <vector>

#include "graphics/dxrenderer.h"
#include "graphics/dxutils.h"

/**
 * Static mesh constructor that takes an already constructed vertex and index
 * buffer.
 */
WaterMesh::WaterMesh( ID3D10Device * pRenderDevice,
					  unsigned int rows,
					  unsigned int cols,
		 			  float spatialStep,
					  float timeStep,
					  float speed,
					  float damping  )
    : mNumRows( rows ),
	  mNumCols( cols ),
	  mVertexCount( 0 ),
      mFaceCount( 0 ),
	  mK1( 0 ),
	  mK2( 0 ),
	  mK3( 0 ),
	  mDamping( damping ),
	  mSpeed( speed ),
	  mTimeStep( timeStep ),
	  mSpatialStep( spatialStep ),
	  mSimulationTime( 0.0f ),
	  mPreviousSolution( rows * cols ),
	  mCurrentSolution( rows * cols ),
      mpVertexBuffer( NULL ),
      mpIndexBuffer( NULL )
{
	init( pRenderDevice );
}

/**
 * Static mesh destructor
 */
WaterMesh::~WaterMesh()
{
    SafeRelease( &mpVertexBuffer );
    SafeRelease( &mpIndexBuffer );
}

/**
 * Takes an array of vertices and indices, uploads them to the video hardware
 * and places their data buffers in mVertexbuffer/mIndexBuffer
 */
void WaterMesh::init( ID3D10Device * pRenderDevice )
{
	const float dx = 1.0f;

	// Initialize a vertex mesh that contains geometry for a plane.
	mVertexCount = mNumRows * mNumCols;
	mFaceCount = ( mNumRows - 1 ) * ( mNumCols - 1 ) * 2;

	// Calculate the simulation constants
	float d = mDamping * mSpatialStep + 2.0f;
	float e = ( mSpeed * mSpeed ) * ( mTimeStep * mTimeStep ) / ( mSpatialStep * mSpatialStep );

	mK1 = ( mDamping * mTimeStep - 2.0f ) / d;
	mK2 = ( 4.0f - 8.0f * e ) / d;
	mK3 = ( 2.0f * e ) / d;

	// Generate the grid vertices for the mesh in system memory.
	std::vector<WaterMeshVertex> vertices( mVertexCount );
	
	float halfWidth  = ( mNumCols - 1 ) * mSpatialStep * 0.5f;
	float halfDepth  = ( mNumRows - 1 ) * mSpatialStep * 0.5f;

	for ( unsigned int i = 0; i < mNumRows; ++i )
	{
		float z = halfDepth - i * mSpatialStep;

		for ( unsigned int j = 0; j < mNumCols; ++j )
		{
			float x = -halfWidth + j * mSpatialStep;

			mPreviousSolution[ i * mNumRows + j ] = D3DXVECTOR3( x, 0.0f, z );
			mCurrentSolution[ i * mNumRows + j ] = D3DXVECTOR3( x, 0.0f, z );
		}
	}

	// Describe the layout of the vertex buffer and create it.
	D3D10_BUFFER_DESC vbd;
	ZeroMemory( &vbd, sizeof(D3D10_BUFFER_DESC) );

	vbd.Usage          = D3D10_USAGE_DYNAMIC;
	vbd.ByteWidth      = sizeof(WaterMeshVertex) * (UINT) vertices.size();
	vbd.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vbd.MiscFlags      = 0;

	D3D10_SUBRESOURCE_DATA vInitData;
	ZeroMemory( &vInitData, sizeof(D3D10_SUBRESOURCE_DATA) );

	vInitData.pSysMem = &vertices[0];

	// Upload the vertex buffer to the graphics card.
	HRESULT result = pRenderDevice->CreateBuffer( &vbd, &vInitData, &mpVertexBuffer );

	DxUtils::CheckResult( result, true, "Creating a vertex buffer" );

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

    DxUtils::CheckResult( result, true, "Creating an index buffer" );
}

/**
 * Simulates ripples on a water surface, and updates the water mesh to match this.
 *
 *  TODO: this is terrible code from the book... rewrite it
 */
void WaterMesh::update( float deltaTime )
{
	static float t = 0.0f;

	// Accumulate time
	t += deltaTime;

	// Only update the simulation at the specified time step
	if ( t >= mTimeStep )
	{
		// Only update interior points; we use zero boundary conditions.
		for ( unsigned int i = 1; i < mNumRows - 1; ++i )
		{
			for ( unsigned int j = 1; j < mNumCols - 1; ++j )
			{
				// Note that j indexes x, and i indexes z.  Our +z axis goes "down" which
				// is to keep consistent with our row indices going down.
				mPreviousSolution[ i * mNumCols + j ].y =
					mK1 * mPreviousSolution[ i * mNumCols + j ].y +
					mK2 * mCurrentSolution[ i * mNumCols + j ].y +
					mK3 * ( mCurrentSolution[ ( i + 1 ) * mNumCols + j ].y +
					        mCurrentSolution[ ( i - 1 ) * mNumCols + j ].y +
							mCurrentSolution[ i * mNumCols + j + 1 ].y +
							mCurrentSolution[ i * mNumCols + j - 1 ].y );
			}
		}

		// We just overwrote the previous buffer with our new data, so this data needs to become
		// the current data, and the current one should become the new previous data.
		mCurrentSolution.swap( mPreviousSolution );

		t = 0.0f;		// reset time (this is stupid... we should accumulate and store it)

		// Update the vertex buffer with the new solution
		WaterMeshVertex * pVertices = NULL;
		DxUtils::CheckResult( mpVertexBuffer->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**) &pVertices ), true, "Uploading new water vertices" );

		for ( unsigned int i = 0; i < mCurrentSolution.size(); ++i )
		{
			pVertices[i].pos = mCurrentSolution[ i ];
			pVertices[i].color = D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f );
		}

		mpVertexBuffer->Unmap();
	}
}

/**
 * Puts a ripple into the water
 */
void WaterMesh::perturb( unsigned int i, unsigned int j, float magnitude )
{
	// Do not disturb boundaries
	assert( i > 1 && i < mNumRows - 2 );
	assert( j > 1 && j < mNumRows - 2 );

	float halfMagnitude = 0.5f * magnitude;

	// Disturb the ijth vertex height and its neighbors
	mCurrentSolution[ i * mNumCols + j ].y         += magnitude;
	mCurrentSolution[ i * mNumCols + j + 1 ].y     += halfMagnitude;
	mCurrentSolution[ i * mNumCols + j - 1 ].y     += halfMagnitude;
	mCurrentSolution[ ( i + 1 ) * mNumCols + j ].y += halfMagnitude;
	mCurrentSolution[ ( i + 1 ) * mNumCols + j ].y += halfMagnitude;
}

/**
 * Render the cube
 */
void WaterMesh::draw( ID3D10Device * pDevice ) const
{
    assert( pDevice != NULL );

    const unsigned int stride = sizeof( WaterMeshVertex );
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