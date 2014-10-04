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
#ifndef SCOTT_HAILSTORM_DEMO_SCENE_H
#define SCOTT_HAILSTORM_DEMO_SCENE_H

#include "runtime\gametime.h"

class DXRenderer;

/**
 * Abstract base class for graphics demo scenes.
 */
class DemoScene
{
public:
    DemoScene() { }
    DemoScene(const DemoScene&) = delete;
    virtual ~DemoScene() { }

    DemoScene& operator =(const DemoScene&) = delete;

    void Initialize(DXRenderer& dx) { OnInitialize(dx); }
    void Update(TimeT currentTime, TimeT deltaTime) { OnUpdate(currentTime, deltaTime); }
    void Render(DXRenderer& dx, TimeT currentTime, TimeT deltaTime) const { OnRender(dx, currentTime, deltaTime); }
    void LoadContent(DXRenderer& dx) { OnLoadContent(dx); }
    void UnloadContent(DXRenderer& dx) { OnUnloadContent(dx); }

protected:
    virtual void OnInitialize(DXRenderer& dx) = 0;
    virtual void OnUpdate(TimeT currentTime, TimeT deltaTime) = 0;
    virtual void OnRender(DXRenderer& dx, TimeT currentTime, TimeT deltaTime) const = 0;
    virtual void OnLoadContent(DXRenderer& dx) = 0;
    virtual void OnUnloadContent(DXRenderer& dx) = 0;
};

#endif