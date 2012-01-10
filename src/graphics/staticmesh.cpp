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
#include "graphics/staticmesh.h"

#include <d3d10_1.h>
#include <d3d10.h>
#include <d3dx10.h>

#include "graphics/dxrenderer.h"
#include "graphics/staticmeshvertex.h"
#include "graphics/dxutils.h"

/**
 * Default static mesh constructor. Creates an empty static mesh that has no
 * faces or vertices
 */
StaticMesh::StaticMesh()
    : mVertexCount( 0 ),
      mFaceCount( 0 ),
      mpVertexBuffer( NULL ),
      mpIndexBuffer( NULL )
{

}

/**
 * Static mesh constructor that takes an already constructed vertex and index
 * buffer.
 */
StaticMesh::StaticMesh( ID3D10Device * pRenderDevice,
                        const StaticMeshVertex *pVertexArray,
                        unsigned int vertexCount,
                        const std::vector<unsigned int>& indexArray )
    : mVertexCount( vertexCount ),
      mFaceCount( static_cast<unsigned int>( indexArray.size() / 3 ) ),
      mpVertexBuffer( NULL ),
      mpIndexBuffer( NULL )
{
    assert( pRenderDevice != NULL );
    assert( (mVertexCount == 0 && mFaceCount == 0        ) || (mVertexCount > 0 && mFaceCount > 0 ) );
    assert( (mVertexCount == 0 && pVertexArray == NULL   ) || pVertexArray != NULL );
    assert( (mFaceCount   == 0 && indexArray.size() == 0 ) || indexArray.size() > 0 );
    assert( (mFaceCount % 3 == 0 ) );

    if ( mFaceCount > 0 )
    {
        uploadMesh( pRenderDevice, pVertexArray, vertexCount, indexArray );
    }
}

/**
 * Static mesh destructor
 */
StaticMesh::~StaticMesh()
{
    SafeRelease( &mpVertexBuffer );
    SafeRelease( &mpIndexBuffer );
}

/**
 * Takes an array of vertices and indices, uploads them to the video hardware
 * and places their data buffers in mVertexbuffer/mIndexBuffer
 */
void StaticMesh::uploadMesh( ID3D10Device * pRenderDevice,
                             const StaticMeshVertex *pVertexArray,
                             unsigned int vertexCount,
                             const std::vector<unsigned int>& indexArray )
{
    assert( vertexCount > 0 );
    assert( indexArray.size() > 0 && (indexArray.size() % 3) == 0 );

    // Construct the vertex buffer by first describing its layout
    D3D10_BUFFER_DESC vbd;
    ZeroMemory( &vbd, sizeof(D3D10_BUFFER_DESC) );
    
    vbd.Usage     = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(StaticMeshVertex) * vertexCount;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    
    D3D10_SUBRESOURCE_DATA vInitData;
    ZeroMemory( &vInitData, sizeof(D3D10_SUBRESOURCE_DATA) );

    vInitData.pSysMem = pVertexArray;

    // Construct the index buffer description
    D3D10_BUFFER_DESC ibd;
    ZeroMemory( &ibd, sizeof(D3D10_BUFFER_DESC) );

    vbd.Usage     = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(DWORD) * mFaceCount * 3;
    vbd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    
    D3D10_SUBRESOURCE_DATA iInitData;
    ZeroMemory( &iInitData, sizeof(D3D10_SUBRESOURCE_DATA) );

    iInitData.pSysMem = &indexArray[0];

    // Now upload both the vertex buffer and index buffer
    HRESULT result = pRenderDevice->CreateBuffer( &vbd, &vInitData, &mpVertexBuffer );

    if (! DxUtils::CheckResult( result, true, "Creating a vertex buffer" ) )
    {
        return;
    }

    result = pRenderDevice->CreateBuffer( &ibd, &iInitData, &mpIndexBuffer );

    if (! DxUtils::CheckResult( result, true, "Creating an index buffer" ) )
    {
        return;
    }

    // Update the instance variables by replacing them with this mesh's values
    mVertexCount = vertexCount;
    mFaceCount   = static_cast<unsigned int>( indexArray.size() / 3 );
}

/**
 * Render dat mesh
 */
void StaticMesh::draw( ID3D10Device * pDevice ) const
{
    assert( pDevice != NULL );

    const unsigned int stride = sizeof( StaticMeshVertex );
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