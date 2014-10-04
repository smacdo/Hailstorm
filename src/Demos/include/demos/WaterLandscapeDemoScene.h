/*
 * Copyright 2014 Scott MacDonald
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
#ifndef SCOTT_HAILSTORM_LANDSCAPE_DEMO_SCENE_H
#define SCOTT_HAILSTORM_LANDSCAPE_DEMO_SCENE_H

#include "graphics/light.h"		// temporary, remove this eventually  (make DXRenderer have a light manager)
#include <d3dx10.h>             // temporary while we host the camera matrices

#include "graphics/DemoScene.h"
#include "runtime\gametime.h"

class LandscapeMesh;
class WaterMesh;

#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

// Forward declarations
struct ID3D10Effect;
class DXRenderer;

/**
 * Chapter 6 demo.
 */
class WaterLandscapeDemoScene : public DemoScene
{
public:
    WaterLandscapeDemoScene();
    WaterLandscapeDemoScene(const WaterLandscapeDemoScene&) = delete;
    virtual ~WaterLandscapeDemoScene() override;

    WaterLandscapeDemoScene& operator =(const WaterLandscapeDemoScene&) = delete;

private:
    virtual void OnInitialize(DXRenderer& dx) override;
    virtual void OnUpdate(TimeT currentTime, TimeT deltaTime) override;
    virtual void OnRender(DXRenderer& dx, TimeT currentTime, TimeT deltaTime) const override;
    virtual void OnLoadContent(DXRenderer& dx) override;
    virtual void OnUnloadContent(DXRenderer& dx) override;

private:
    void BuildLights();
    void BuildInputLayout(DXRenderer& dx);

private:
    ID3D10InputLayout * mpVertexLayout;

    D3DXMATRIX mView;
    
    D3DXVECTOR3 mEyePos;

    D3DXMATRIX mLandTransform;
    D3DXMATRIX mWaterTransform;

    Light mLights[3];
    int mLightType;
    float mRadius;

    LandscapeMesh * mpCubeMesh;
    WaterMesh * mpWaterMesh;

private:
    Microsoft::WRL::ComPtr<ID3D10Effect> mLandscapeEffect;
};

#endif