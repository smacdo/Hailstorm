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

#include "graphics/staticmesh.h"
#include "graphics/staticmeshvertex.h"

/**
 * Generates a new static mesh for a 3d box
 */
StaticMesh* MeshFactory::CreateBox( ID3D10Device *pRenderDevice, float scale )
{
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

    return new StaticMesh( pRenderDevice, VERTICES, VERTEX_COUNT, indices );
}