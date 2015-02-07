/*
 * Copyright 2015 Scott MacDonald
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
#include "SceneRenderer.h"
#include "Rendering/Light.h"

#include <array>
#include <memory>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>

// Forward declarations
class Camera;
class LandscapeMesh;
class WaterMesh;

struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

namespace Hailstorm
{
    class StepTimer;
    class Camera;
}

namespace HailstormApp
{
    class LandscapeDemoSceneRenderer : public SceneRenderer
    {
    public:
        LandscapeDemoSceneRenderer(std::shared_ptr<Hailstorm::DeviceResources> deviceResources);
        virtual ~LandscapeDemoSceneRenderer() override;

        // Initializes view parameters when the window size changes.
        virtual void CreateDeviceDependentResources() override;
        virtual void CreateWindowSizeDependentResources() override;
        virtual void ReleaseDeviceDependentResources() override;
        virtual void Update(const Hailstorm::StepTimer& timer) override;
        virtual void Render() override;

    protected:
        virtual void OnTrackingUpdate(float positionX) override;

    private:
        void UpdateInput();
        void GenerateRandomWave();

        void BuildLights();
        void BuildInputLayout();

    private:
        bool mLoadingComplete;

        Microsoft::WRL::ComPtr<ID3D11InputLayout> mVertexLayout;
        std::shared_ptr<Hailstorm::Camera> mCamera;

        std::array<Hailstorm::Light, 3> mLights;
        int mLightType;

        std::unique_ptr<LandscapeMesh> mTerrainMesh;
        std::unique_ptr<WaterMesh> mWaterMesh;
    };

}
