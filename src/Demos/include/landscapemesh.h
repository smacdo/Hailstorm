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
#ifndef SCOTT_HAILSTORM_LANDSCAPE_MESH_H
#define SCOTT_HAILSTORM_LANDSCAPE_MESH_H

// Includes
#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

// Forward declarations
struct ID3D10Buffer;
struct ID3D10Device;
struct StaticMeshVertex;
struct D3DXCOLOR;

/**
 * Contains information on rendering a cube mesh.
 */
class LandscapeMesh
{
public:
    LandscapeMesh( ID3D10Device * pRenderDevice,
		           unsigned int rows,
				   unsigned int cols,
				   float spatialStep );
    LandscapeMesh(const LandscapeMesh&) = delete;
    virtual ~LandscapeMesh();

    const LandscapeMesh& operator =(const LandscapeMesh&) = delete;

    void Draw( ID3D10Device *pDevice ) const;

    unsigned int VertexCount() const { return mVertexCount; }
    unsigned int FaceCount() const { return mFaceCount; }
	float GetHeight( float x, float y ) const;

private:
	void Init( ID3D10Device * pDevice, float dx );
    static void FindVertexColor(float y, D3DXCOLOR * pDiffuse, D3DXCOLOR * pSpecular);

private:
	unsigned int mNumRows;
	unsigned int mNumCols;
    unsigned int mVertexCount;
    unsigned int mFaceCount;
    Microsoft::WRL::ComPtr<ID3D10Buffer> mVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D10Buffer> mIndexBuffer;
};

#endif