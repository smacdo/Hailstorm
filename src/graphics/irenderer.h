/*
 * Copyright (C) 2011 Scott MacDonald. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SCOTT_HAILSTORM_GRAPHICS_IRENDERER_H
#define SCOTT_HAILSTORM_GRAPHICS_IRENDERER_H

#include <boost/utility.hpp>
class Window;

/**
 * This is an abstract renderer
 */
class IRenderer : boost::noncopyable
{
public:
    explicit IRenderer( Window *pWindow );
    virtual ~IRenderer();

    bool initialize();
    void stop();
    void tick();

protected:
    virtual bool onStartRenderer() = 0;
    virtual void onStopRenderer() = 0;
    virtual void onRenderFrame( float currentTime, float deltaTime ) = 0;

    Window *renderWindow();

private:
    Window *mpWindow;
    bool mRendererCreatedAndRunning;
};

#endif