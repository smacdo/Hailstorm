/*
 * Copyright 2015 Scott MacDonald
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

namespace Hailstorm
{
    class StepTimer;

    /**
     * Abstract base class for cameras.
     */
    class Camera
    {
    public:
        Camera();
        virtual ~Camera();

        // Get the camera position.
        virtual DirectX::XMFLOAT3 Position() const = 0;

        // TODO: Is this actually useful? Consider removing it.
        virtual DirectX::XMFLOAT3 Target() const = 0;

        // Retrieve the view matrix.
        virtual DirectX::XMMATRIX GetViewMatrix() const = 0;

        // Retrieve the projection matrix.
        virtual DirectX::XMMATRIX GetProjectionMatrix() const = 0;

        // Set camera lens information.
        virtual void SetLens(float fovY, float aspect, float zNear, float zFar) = 0;

        // Update camera view matrices and whatnot.
        virtual void Update(const StepTimer& time) = 0;

        // -> DESCRIBE ME.
        // TODO: This is kinda hacky. I mean... it works for the moment, but it doesn't feel right.
        virtual void UpdateWithInput(const StepTimer& time) = 0;
    };

    /**
     * A simple rotational camera based on Frank Luna's Learning DirectX 10 samples.
     */
    class RotationalCamera : public Camera
    {
    public:
        RotationalCamera();

        virtual DirectX::XMFLOAT3 Position() const override;
        virtual DirectX::XMFLOAT3 Target() const override;
        virtual DirectX::XMMATRIX GetViewMatrix() const override;
        virtual DirectX::XMMATRIX GetProjectionMatrix() const override;

        virtual void SetLens(float fovY, float aspect, float zNear, float zFar) override;

        virtual void Update(const StepTimer& time) override;
        virtual void UpdateWithInput(const StepTimer& time) override;

    private:
        bool mNeedToUpdateViewMatrix;
        float mRadius;
        mutable DirectX::XMMATRIX mView;
        DirectX::XMMATRIX mProjection;
        DirectX::XMFLOAT3 mEyePos;
        DirectX::XMFLOAT3 mTarget;
        DirectX::XMFLOAT3 mUp;
    };
}


