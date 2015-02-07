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

namespace Hailstorm
{
    /**
     * Holds lighting information needed for rendering a 3d scene.
     */
    class Light
    {
    public:
        Light();

        void SetPosition(DirectX::XMFLOAT3 position);
        void SetDirection(DirectX::XMFLOAT3 direction);
        void SetAmbientColor(DirectX::XMFLOAT4 color);
        void SetDiffuseColor(DirectX::XMFLOAT4 color);
        void SetSpecularColor(DirectX::XMFLOAT4 color);
        void SetAttentuation(DirectX::XMFLOAT3 color);
        void SetSpotLightPower(float power);
        void SetRange(float range);

        DirectX::XMFLOAT3 Position() const;
        DirectX::XMFLOAT3 Direction() const;
        DirectX::XMFLOAT4 AmbientColor() const;
        DirectX::XMFLOAT4 DiffuseColor() const;
        DirectX::XMFLOAT4 SpecularColor() const;
        DirectX::XMFLOAT3 Attentuation() const;
        float SpotLightPower() const;
        float Range() const;

    private:
        DirectX::XMFLOAT3 mPosition;
        DirectX::XMFLOAT3 mDirection;
        DirectX::XMFLOAT4 mAmbientColor;
        DirectX::XMFLOAT4 mDiffuseColor;
        DirectX::XMFLOAT4 mSpecularColor;
        DirectX::XMFLOAT3 mAttenuation;
        float mPower;
        float mRange;
    };
}

