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
#ifndef SCOTT_HAILSTORM_ROTATIONAL_CAMERA_H
#define SCOTT_HAILSTORM_ROTATIONAL_CAMERA_H

#include <d3dx10.h>
#include "Camera.h"

/**
 * A simple rotational camera based on Frank Luna's Learning DirectX 10 samples.
 */
class RotationalCamera : public Camera
{
public:
    RotationalCamera();

    virtual D3DXVECTOR3 Position() const override;
    virtual D3DXVECTOR3 Target() const override;
    virtual D3DXMATRIX GetViewMatrix() const override;
    virtual D3DXMATRIX GetProjectionMatrix() const override;
    
    virtual void SetLens(float fovY, float aspect, float zNear, float zFar) override;

    virtual void Update(TimeT currentTime, TimeT deltaTime) override;
    virtual void UpdateWithInput(TimeT currentTime, TimeT deltaTime) override;

private:
    bool mNeedToUpdateViewMatrix;
    float mRadius;
    mutable D3DXMATRIX mView;
    D3DXMATRIX mProjection;
    D3DXVECTOR3 mEyePos;
    D3DXVECTOR3 mTarget;
    D3DXVECTOR3 mUp;
};

#endif