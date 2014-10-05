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
#ifndef SCOTT_HAILSTORM_GRAPHICS_STATIC_MESH
#define SCOTT_HAILSTORM_GRAPHICS_STATIC_MESH

#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

// Forward declarations
struct ID3D10Buffer;
struct ID3D10Device;
struct StaticMeshVertex;

/**
 * [DESCRIPTION]
 */
// TODO: Support copying?
class StaticMesh
{
public:
    StaticMesh();
    StaticMesh( ID3D10Device * pRenderDevice,
                const StaticMeshVertex * pVertexArray,
                unsigned int vertexCount,
                const std::vector<unsigned int>& indexArray );
    StaticMesh(const StaticMesh&) = delete;
    ~StaticMesh();

    StaticMesh& operator =(const StaticMesh&) = delete;

    void draw( ID3D10Device *pDevice ) const;

    unsigned int vertexCount() const;
    unsigned int faceCount() const;

private:
    void uploadMesh( ID3D10Device * pRenderDevice,
                     const StaticMeshVertex * pVertexArray,
                     unsigned int vertexCount,
                     const std::vector<unsigned int>& indexArray );

private:
    unsigned int mVertexCount;
    unsigned int mFaceCount;
    Microsoft::WRL::ComPtr<ID3D10Buffer> mVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D10Buffer> mIndexBuffer;
};

#endif