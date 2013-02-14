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
#ifndef SCOTT_HAILSTORM_WATER_MESH_H
#define SCOTT_HAILSTORM_WATER_MESH_H

// Includes
#include <boost/noncopyable.hpp>
#include <vector>
#include <d3dx10.h>

// Forward declarations
struct ID3D10Buffer;
struct ID3D10Device;
struct StaticMeshVertex;

/**
 * The vertex structure that is used in the static mesh
 */
struct WaterMeshVertex
{
    D3DXVECTOR3 pos;
    D3DXCOLOR color;
};

/**
 * Contains information on rendering a water plane with ripples.
 */
class WaterMesh : boost::noncopyable
{
public:
    WaterMesh( ID3D10Device * pRenderDevice,
		       unsigned int rows,
			   unsigned int cols,
			   float spatialStep,
			   float timeStep,
			   float speed,
			   float damping );
    ~WaterMesh();

    void draw( ID3D10Device *pDevice ) const;

    unsigned int vertexCount() const;
    unsigned int faceCount() const;

	void perturb( unsigned int i, unsigned int j, float magnitude );
	void update( float deltaTime );

private:
	void init( ID3D10Device * pDevice );
	

private:
	unsigned int mNumRows;
	unsigned int mNumCols;
    unsigned int mVertexCount;
    unsigned int mFaceCount;

	// Simulation constants
	float mK1, mK2, mK3;

	float mDamping;
	float mSpeed;
	float mTimeStep;
	float mSpatialStep;
	float mSimulationTime;

	std::vector<D3DXVECTOR3> mPreviousSolution;
	std::vector<D3DXVECTOR3> mCurrentSolution;

    ID3D10Buffer * mpVertexBuffer;
    ID3D10Buffer * mpIndexBuffer;
};

#endif