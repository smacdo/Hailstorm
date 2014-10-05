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
#ifndef SCOTT_HAILSTORM_GRAPHICS_STATIC_MESH_FACTORY
#define SCOTT_HAILSTORM_GRAPHICS_STATIC_MESH_FACTORY

#include <string>
#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

// Forward declarations
class StaticMesh;
struct ID3D10Device;
struct ID3D10Effect;
struct ID3D10EffectTechnique;
struct ID3D10InputLayout;

/**
 * Creates simple geometric static meshes at run time
 */
class MeshFactory
{
public:
    MeshFactory(const std::wstring& dataDir, ID3D10Device * pRenderDevice);
    MeshFactory(const MeshFactory&) = delete;
    ~MeshFactory();

    MeshFactory operator =(const MeshFactory&) = delete;

    std::shared_ptr<StaticMesh> createBox( float scale ) const;

private:
    void Init(const std::wstring& dataDir);

private:
    Microsoft::WRL::ComPtr<ID3D10Device> mRenderDevice;
    Microsoft::WRL::ComPtr<ID3D10Effect> mStaticMeshFX;
    Microsoft::WRL::ComPtr<ID3D10InputLayout> mStaticMeshInputLayout;
    ID3D10EffectTechnique * mpStaticMeshTechnique;
};

#endif