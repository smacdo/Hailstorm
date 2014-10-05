/*
 * Copyright 2011 Scott MacDonald
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
#ifndef SCOTT_HAILSTORM_CAMERA_H
#define SCOTT_HAILSTORM_CAMERA_H

#include <d3dx10.h>
#include "runtime/gametime.h"

/**
 * Abstract base class for cameras.
 */
class Camera
{
public:

    // Get the camera position.
    virtual D3DXVECTOR3 Position() const = 0;

    // TODO: Is this actually useful? Consider removing it.
    virtual D3DXVECTOR3 Target() const = 0;

    // Retrieve the view matrix.
    virtual D3DXMATRIX GetViewMatrix() const = 0;

    // Retrieve the projection matrix.
    virtual D3DXMATRIX GetProjectionMatrix() const = 0;

    // Set camera lens information.
    virtual void SetLens(float fovY, float aspect, float zNear, float zFar) = 0;

    // Update camera view matrices and whatnot.
    virtual void Update(TimeT currentTime, TimeT deltaTime) = 0;
    
    // -> DESCRIBE ME.
    // TODO: This is kinda hacky. I mean... it works for the moment, but it doesn't feel right.
    virtual void UpdateWithInput(TimeT currentTime, TimeT deltaTime) = 0;
};

#endif