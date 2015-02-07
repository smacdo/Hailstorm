/*
 * Copyright 2011 - 2015 Scott MacDonald
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
#pragma once

// Forward declarations
struct ID3D10Buffer;
struct ID3D10Device;
struct StaticMeshVertex;

namespace Hailstorm
{

    /**
    * The vertex structure that is used in the static mesh
    */
    struct WaterMeshVertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT4 diffuse;
        DirectX::XMFLOAT4 spec; // (r, g, b, specPower);
    };

    /**
     * Represents a water surface with ripples.
     */
    class WaterMesh
    {
    public:
        WaterMesh(ID3D11Device * pRenderDevice,
                  unsigned int rows,
                  unsigned int cols,
                  float spatialStep,
                  float timeStep,
                  float speed,
                  float damping);
        WaterMesh(const WaterMesh&) = delete;
        ~WaterMesh();

        WaterMesh& operator =(const WaterMesh&) = delete;

        void Draw(ID3D11Device *pDevice) const;

        unsigned int VertexCount() const { return mVertexCount; }
        unsigned int FaceCount() const { return mFaceCount; }

        void Perturb(unsigned int i, unsigned int j, float magnitude);

        void Update(float deltaTime);

    private:
        void Init(ID3D11Device * pDevice);
        void UpdateGrid();
        void UpdateNormals();
        void UpdateVertexBuffer();

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

        std::unique_ptr<DirectX::XMFLOAT3[]> mPreviousSolution;
        std::unique_ptr<DirectX::XMFLOAT3[]> mCurrentSolution;
        std::unique_ptr<DirectX::XMFLOAT3[]> mNormals;

        Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
    };
}

