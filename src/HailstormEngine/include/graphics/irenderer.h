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

class IWindow;

/**
 * This is an abstract renderer
 */
class IRenderer
{
public:
    explicit IRenderer( IWindow *pWindow );
    IRenderer(const IRenderer&) = delete;
    virtual ~IRenderer();

    IRenderer& operator =(const IRenderer&) = delete;

    bool initialize();
    void stop();
    void tick( TimeT currentTime, TimeT deltaTime );

protected:
    virtual bool onStartRenderer() = 0;
    virtual void onStopRenderer() = 0;
	virtual void onUpdate( double currentTime, double deltaTime ) = 0;
    virtual void onRenderFrame() = 0;
    virtual bool resizeRenderWindow( unsigned int width, unsigned int height ) = 0;

    IWindow *renderWindow();

private:
    IWindow *mpWindow;
    bool mRendererCreatedAndRunning;
};

#endif