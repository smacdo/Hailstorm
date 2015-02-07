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
#include "Camera.h"

#include "Common/StepTimer.h"

using namespace Hailstorm;
using namespace DirectX;

Camera::Camera()
{
}

Camera::~Camera()
{
}

RotationalCamera::RotationalCamera()
    : mNeedToUpdateViewMatrix(false),
      mRadius(75),
      mView(),
      mProjection(),
      mEyePos(),
      mTarget(0.0f, 0.0f, 0.0f),
      mUp(0.0f, 1.0f, 0.0f)
{
    mView = XMMatrixIdentity();
    mProjection = XMMatrixIdentity();
}

XMFLOAT3 RotationalCamera::Position() const
{
    return mEyePos;
}

XMFLOAT3 RotationalCamera::Target() const
{
    return mTarget;
}

XMMATRIX RotationalCamera::GetViewMatrix() const
{
    return mView;
}

XMMATRIX RotationalCamera::GetProjectionMatrix() const
{
    return mProjection;
}

void RotationalCamera::SetLens(float fovY, float aspect, float zNear, float zFar)
{
    mProjection = XMMatrixPerspectiveFovLH(fovY, aspect, zNear, zFar);
}

void RotationalCamera::Update(const StepTimer& time)
{
    // Rotate camera around the landscape
    mEyePos.x = mRadius * cosf(static_cast<float>(0.5 * time.GetTotalSeconds()));
    mEyePos.z = mRadius * sinf(static_cast<float>(0.5 * time.GetTotalSeconds()));
    mEyePos.y = 50.0f * sinf(static_cast<float>(0.5 * time.GetTotalSeconds())) + 75.0f;

    // Rebuild view matrix with the new camera coordinates
    // Update view matrix.
    mView = XMMatrixLookAtLH(
        XMLoadFloat3(&mEyePos),
        XMLoadFloat3(&mTarget),
        XMLoadFloat3(&mUp));
}

void RotationalCamera::UpdateWithInput(const StepTimer& time)
{
    /*
    -- example --

    if (GetKeyState('1' & 0x8000)) { mLightType = 0; LOG_DEBUG("Renderer") << "Switched to light type 0"; }
    if (GetKeyState('2' & 0x8000)) { mLightType = 1; LOG_DEBUG("Renderer") << "Switched to light type 1"; }
    if (GetKeyState('3' & 0x8000)) { mLightType = 2; LOG_DEBUG("Renderer") << "Switched to light type 2"; }
    */
}