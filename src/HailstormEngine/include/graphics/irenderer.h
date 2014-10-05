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
#ifndef SCOTT_HAILSTORM_GRAPHICS_IRENDERER_H
#define SCOTT_HAILSTORM_GRAPHICS_IRENDERER_H

#include "runtime/gametime.h"
#include <memory>                       // Shared pointers.
#include <wrl\wrappers\corewrappers.h>  // ComPtr.
#include <wrl\client.h>                 // ComPtr friends.

class IWindow;
class DemoScene;
struct Size;

/**
 * This is an abstract renderer
 */
class IRenderer
{
public:
    explicit IRenderer(std::shared_ptr<IWindow> window);
    IRenderer(const IRenderer&) = delete;
    virtual ~IRenderer();

    IRenderer& operator =(const IRenderer&) = delete;

    void initialize();
    void stop();
    void Update(const DemoScene& demo, TimeT currentTime, TimeT deltaTime);

protected:
    virtual void OnStartRenderer() = 0;
    virtual void OnStopRenderer() = 0;
    
    virtual void OnRenderFrame(const DemoScene& demo, TimeT currentTime, TimeT deltaTime) = 0;

    virtual void OnWindowResized(const Size& screenSize) = 0;

    std::shared_ptr<IWindow> GetWindow() { return mWindow; }
    std::shared_ptr<const IWindow> GetWindow() const { return mWindow; }

private:
    std::shared_ptr<IWindow> mWindow;
    bool mRendererCreatedAndRunning;
};

#endif