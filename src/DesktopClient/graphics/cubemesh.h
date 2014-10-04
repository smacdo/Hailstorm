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
#ifndef SCOTT_HAILSTORM_CUBE_MESH_H
#define SCOTT_HAILSTORM_CUBE_MESH_H

// Includes
#include <boost/noncopyable.hpp>

// Forward declarations
struct ID3D10Buffer;
struct ID3D10Device;
struct StaticMeshVertex;

/**
 * Contains information on rendering a cube mesh.
 */
class CubeMesh : boost::noncopyable
{
public:
    CubeMesh( ID3D10Device * pRenderDevice );
    ~CubeMesh();

    void draw( ID3D10Device *pDevice ) const;

    unsigned int vertexCount() const;
    unsigned int faceCount() const;

private:
	void init( ID3D10Device * pDevice );

private:
    unsigned int mVertexCount;
    unsigned int mFaceCount;
    ID3D10Buffer * mpVertexBuffer;
    ID3D10Buffer * mpIndexBuffer;
};

#endif