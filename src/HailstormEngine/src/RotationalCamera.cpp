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
#include "stdafx.h"
#include "camera/RotationalCamera.h"
#include "runtime/logging.h"
#include "runtime/Size.h"
#include "graphics/DirectXExceptions.h"
#include "camera/RotationalCamera.h"

#include <DXGI.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

RotationalCamera::RotationalCamera()
    : mNeedToUpdateViewMatrix(false),
      mRadius(75),
      mView(),
      mProjection(),
      mEyePos(),
      mTarget(0.0f, 0.0f, 0.0f),
      mUp(0.0f, 1.0f, 0.0f)
{
    D3DXMatrixIdentity(&mView);
    D3DXMatrixIdentity(&mProjection);
}

D3DXVECTOR3 RotationalCamera::Position() const
{
    return mEyePos;
}

D3DXVECTOR3 RotationalCamera::Target() const
{
    return mTarget;
}

D3DXMATRIX RotationalCamera::GetViewMatrix() const
{
    return mView;
}

D3DXMATRIX RotationalCamera::GetProjectionMatrix() const
{
    return mProjection;
}

void RotationalCamera::SetLens(float fovY, float aspect, float zNear, float zFar)
{
    D3DXMatrixPerspectiveFovLH(&mProjection, fovY, aspect, zNear, zFar);
}

void RotationalCamera::Update(TimeT currentTime, TimeT deltaTime)
{
    // Rotate camera around the landscape
    mEyePos.x = mRadius * cosf(static_cast<float>(0.5 * currentTime));
    mEyePos.z = mRadius * sinf(static_cast<float>(0.5 * currentTime));
    mEyePos.y = 50.0f * sinf(static_cast<float>(0.5 * currentTime)) + 75.0f;

    // Rebuild view matrix with the new camera coordinates
    

    // Update view matrix.
    D3DXMatrixLookAtLH(&mView, &mEyePos, &mTarget, &mUp);
}

void RotationalCamera::UpdateWithInput(TimeT currentTime, TimeT deltaTime)
{
    /*
    -- example --
    
    if (GetKeyState('1' & 0x8000)) { mLightType = 0; LOG_DEBUG("Renderer") << "Switched to light type 0"; }
    if (GetKeyState('2' & 0x8000)) { mLightType = 1; LOG_DEBUG("Renderer") << "Switched to light type 1"; }
    if (GetKeyState('3' & 0x8000)) { mLightType = 2; LOG_DEBUG("Renderer") << "Switched to light type 2"; }
    */
}