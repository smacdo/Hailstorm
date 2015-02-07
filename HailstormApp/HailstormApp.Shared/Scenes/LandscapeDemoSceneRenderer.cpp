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
#include "pch.h"
#include "LandscapeDemoSceneRenderer.h"
#include "Common/DirectXHelper.h"
#include "Common/Camera.h"

using namespace HailstormApp;
using namespace Hailstorm;

using namespace DirectX;
using namespace Windows::Foundation;

/**
* Loads vertex and pixel shaders from files and instantiates the cube geometry.
*/
LandscapeDemoSceneRenderer::LandscapeDemoSceneRenderer(std::shared_ptr<DeviceResources> deviceResources)
    : SceneRenderer(),
      mLoadingComplete(false)
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

/**
* Destructor.
*/
LandscapeDemoSceneRenderer::~LandscapeDemoSceneRenderer()
{
}

/**
* Initializes view parameters when the window size changes.
*/
void LandscapeDemoSceneRenderer::CreateWindowSizeDependentResources()
{
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void LandscapeDemoSceneRenderer::Update(const StepTimer& timer)
{
}

// Renders one frame using the vertex and pixel shaders.
void LandscapeDemoSceneRenderer::Render()
{
}

void LandscapeDemoSceneRenderer::CreateDeviceDependentResources()
{
}

void LandscapeDemoSceneRenderer::ReleaseDeviceDependentResources()
{
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void LandscapeDemoSceneRenderer::OnTrackingUpdate(float positionX)
{
}