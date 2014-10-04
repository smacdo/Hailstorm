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
#include "stdafx.h"
#include "demos\WaterLandscapeDemoScene.h"

#include "graphics/dxrenderer.h"
#include "graphics/dxutils.h"
#include "graphics/graphicscontentmanager.h"
#include "landscapemesh.h"
#include "watermesh.h"

#include "runtime/debugging.h"
#include "runtime/gametime.h"
#include "runtime/logging.h"
#include "runtime/delete.h"
#include "runtime/mathutils.h"

#include <DXGI.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <algorithm>

#undef max

WaterLandscapeDemoScene::WaterLandscapeDemoScene()
    : DemoScene(),
      mpVertexLayout(),                 // TODO: Ensure this init list follows what is in class.
      mEyePos(0.0f, 0.0f, 0.0f),
      mRadius(75),
      mLightType(0),
      mpCubeMesh(NULL),
      mpWaterMesh(NULL)
{
    D3DXMatrixIdentity(&mView);
    D3DXMatrixIdentity(&mLandTransform);
    D3DXMatrixIdentity(&mWaterTransform);
}

WaterLandscapeDemoScene::~WaterLandscapeDemoScene()
{
    SafeRelease(&mpVertexLayout);
    Delete(mpCubeMesh);
}

void WaterLandscapeDemoScene::OnInitialize(DXRenderer& dx)
{
    HRESULT hr = dx.LoadFxFile(L"../data/shaders/landscape.fx", &mLandscapeEffect);

    BuildLights();
    BuildInputLayout(dx);

    mpCubeMesh = new LandscapeMesh(dx.GetDevice(), 129, 129, 1.0f);
    mpWaterMesh = new WaterMesh(dx.GetDevice(), 257, 257, 0.5f, 0.03f, 3.25f, 0.4f);
}

void WaterLandscapeDemoScene::OnUpdate(TimeT currentTime, TimeT deltaTime)
{
    // Set up the light type based on user input
    if (GetKeyState('1' & 0x8000)) { mLightType = 0; LOG_DEBUG("Renderer") << "Switched to light type 0"; }
    if (GetKeyState('2' & 0x8000)) { mLightType = 1; LOG_DEBUG("Renderer") << "Switched to light type 1"; }
    if (GetKeyState('3' & 0x8000)) { mLightType = 2; LOG_DEBUG("Renderer") << "Switched to light type 2"; }

    // Every quarter second, generate a random wave
    static float t_base = 0.0f;

    if (currentTime - t_base >= 0.25f)
    {
        t_base += 0.25f;

        unsigned int i = 5 + rand() % 250;
        unsigned int j = 5 + rand() % 250;

        float r = randF(1.0f, 2.0f);

        mpWaterMesh->Perturb(i, j, r);
    }

    // Make sure the water mesh is kept up to date with ripple animations
    mpWaterMesh->Update(deltaTime);

    // Rotate camera around the landscape
    mEyePos.x = mRadius * cosf(static_cast<float>(0.5 * currentTime));
    mEyePos.z = mRadius * sinf(static_cast<float>(0.5 * currentTime));
    mEyePos.y = 50.0f * sinf(static_cast<float>(0.5 * currentTime)) + 75.0f;

    // Rebuild view matrix with the new camera coordinates
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);


    // The point light circles the scene as a function of time, staying seven units above the land's
    // or water's surface.
    mLights[1].pos.x = 50.0f * cosf((float)currentTime);
    mLights[1].pos.z = 50.0f * sinf((float)currentTime);
    mLights[1].pos.y = 7.0f + std::max(mpCubeMesh->GetHeight(mLights[1].pos.x, mLights[1].pos.z), 0.0f);

    // The spotlight takes on the camera position and is aimed in the same direction as the camera is
    // looking. In this way it looks like we are holding a flashlight.
    mLights[2].pos = mEyePos;
    D3DXVec3Normalize(&mLights[2].dir, &(target - mEyePos));
}

void WaterLandscapeDemoScene::OnRender(DXRenderer& dx, TimeT currentTime, TimeT deltaTime) const
{
    // Set the device up for rendering our landscape mesh.
    dx.GetDevice()->IASetInputLayout(mpVertexLayout);
    dx.GetDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3DXMATRIX projectionMatrix;
    dx.GetProjectionMatrix(&projectionMatrix);
    

    // Load the landscape technique.
    ID3D10EffectTechnique * pTechnique = mLandscapeEffect->GetTechniqueByName("LandscapeTechnique");

    // Grab the shader variables we'll need.
    ID3D10EffectMatrixVariable * pWVP = mLandscapeEffect->GetVariableByName("gWVP")->AsMatrix();
    ID3D10EffectMatrixVariable * pWorldVar = mLandscapeEffect->GetVariableByName("gWorld")->AsMatrix();
    ID3D10EffectVariable * pFxEyePosVar = mLandscapeEffect->GetVariableByName("gEyePosW");
    ID3D10EffectVariable * pFxLightVar = mLandscapeEffect->GetVariableByName("gLight");
    ID3D10EffectScalarVariable * pFxLightType = mLandscapeEffect->GetVariableByName("gLightType")->AsScalar();

    // Set per frame constants
    D3DXVECTOR3 eyePos = mEyePos;
    Light selectedLight = mLights[mLightType];

    pFxEyePosVar->SetRawValue(&eyePos, 0, sizeof(D3DXVECTOR3));
    pFxLightVar->SetRawValue(&selectedLight, 0, sizeof(Light));
    pFxLightType->SetInt(mLightType);

    // Load the effect technique for cube
    D3D10_TECHNIQUE_DESC technique;
    pTechnique->GetDesc(&technique);

    // Apply the landscape technique.
    for (unsigned int passIndex = 0; passIndex < technique.Passes; ++passIndex)
    {
        ID3D10EffectPass * pPass = pTechnique->GetPassByIndex(passIndex);
        dx.SetDefaultRendering();

        // Draw the landscape mesh first
        D3DXMATRIX wvp = mLandTransform * mView * projectionMatrix;

        pWVP->SetMatrix((float*)&wvp);
        pWorldVar->SetMatrix((float*)&mLandTransform);

        pPass->Apply(0);
        mpCubeMesh->Draw(dx.GetDevice());

        // Draw the water mesh
        wvp = mWaterTransform * mView * projectionMatrix;

        pWVP->SetMatrix((float*)&wvp);
        pWorldVar->SetMatrix((float*)&mWaterTransform);

        // SetWireframeRendering();
        pPass->Apply(0);

        mpWaterMesh->Draw(dx.GetDevice());
    }
}

void WaterLandscapeDemoScene::OnLoadContent(DXRenderer& dx)
{

}

void WaterLandscapeDemoScene::OnUnloadContent(DXRenderer& dx)
{

}

void WaterLandscapeDemoScene::BuildLights()
{
    mLights[0].dir = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
    mLights[0].ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
    mLights[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    mLights[0].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    // Point light (position is changed every frame)
    mLights[1].ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
    mLights[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    mLights[1].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    mLights[1].att.x = 0.0f;
    mLights[1].att.y = 0.1f;
    mLights[1].att.z = 0.0f;
    mLights[1].range = 50.0f;

    // Spotlight -- position and direction changed every frame
    mLights[2].ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
    mLights[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    mLights[2].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    mLights[2].att.x = 1.0f;
    mLights[2].att.y = 0.0f;
    mLights[2].att.z = 0.0f;
    mLights[2].spotPow = 64.0f;
    mLights[2].range = 10000.0f;
}

void WaterLandscapeDemoScene::BuildInputLayout(DXRenderer& dx)
{
    // Describe the vertex input layout.
    D3D10_INPUT_ELEMENT_DESC vertexDescription[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "DIFFUSE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0 }
    };

    // Load the default pass from the .fx file we loaded earlier
    D3D10_PASS_DESC passDescription;
    ID3D10EffectTechnique * pTechnique = mLandscapeEffect->GetTechniqueByName("LandscapeTechnique");

    VerifyNotNull(pTechnique);

    pTechnique->GetPassByIndex(0)->GetDesc(&passDescription);

    // Create the vertex input layout.
    HRESULT result = dx.GetDevice()->CreateInputLayout(vertexDescription,
                                                    4,
                                                    passDescription.pIAInputSignature,
                                                    passDescription.IAInputSignatureSize,
                                                    &mpVertexLayout);
    // Make sure it worked
    DXRenderer::verifyResult(result, "Creating the vertex input layout");
    LOG_DEBUG("Renderer") << "Created the vertex input layout.";
}