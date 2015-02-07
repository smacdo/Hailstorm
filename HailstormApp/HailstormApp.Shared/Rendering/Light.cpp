#include "pch.h"
#include "Light.h"

using namespace Hailstorm;
using namespace DirectX;

Light::Light()
    : mPosition(),
      mDirection(),
      mAmbientColor(),
      mDiffuseColor(),
      mSpecularColor(),
      mAttenuation(),
      mPower(),
      mRange()
{
}

void Light::SetPosition(XMFLOAT3 position)
{
    mPosition = position;
}

void Light::SetDirection(XMFLOAT3 direction)
{
    mDirection = direction;
}

void Light::SetAmbientColor(XMFLOAT4 color)
{
    mAmbientColor = color;
}

void Light::SetDiffuseColor(XMFLOAT4 color)
{
    mDiffuseColor = color;
}

void Light::SetSpecularColor(XMFLOAT4 color)
{
    mSpecularColor = color;
}

void Light::SetAttentuation(XMFLOAT3 color)
{
    mAttenuation = color;
}

void Light::SetSpotLightPower(float power)
{
    mPower = power;
}

void Light::SetRange(float range)
{
    mRange = range;
}


XMFLOAT3 Light::Position() const
{
    return mPosition;
}

XMFLOAT3 Light::Direction() const
{
    return mDirection;
}

XMFLOAT4 Light::AmbientColor() const
{
    return mAmbientColor;
}

XMFLOAT4 Light::DiffuseColor() const
{
    return mDiffuseColor;
}

XMFLOAT4 Light::SpecularColor() const
{
    return mSpecularColor;
}

XMFLOAT3 Light::Attentuation() const
{
    return mAttenuation;
}

float Light::SpotLightPower() const
{
    return mPower;
}

float Light::Range() const
{
    return mRange;
}