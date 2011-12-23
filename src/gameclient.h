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
#ifndef SCOTT_GAME_CLIENT_H
#define SCOTT_GAME_CLIENT_H

#include <boost/noncopyable.hpp>
#include "common/platform.h"
#include "common/gametime.h"

class MainWindow;
class IRenderer;

/**
 * This is the foundation class for a hailstorm game client. A custom game
 * shall inherit from this class to provide custom game play and functionality
 */
class GameClient : boost::noncopyable
{
public:
    GameClient( MainWindow *pMainWindow );
    virtual ~GameClient();

    App::EProgramStatus run();
    void setUpdateFrequency( int numUpdatesPerSecond );

protected:
    virtual bool initialize();
    virtual bool loadContent();
    virtual void unloadContent();
    virtual void update( TimeT simulationTime, TimeT deltaTime );
    virtual void draw( TimeT simulationTime, float interpolation );

private:
    App::EProgramStatus runMainGameLoop();
    TimeT getCurrentTime() const;
    void calculateSystemTimerFrequency();
    bool initializeClient();

private:
    /// Pointer to the main rendering window
    MainWindow * mpMainWindow;

    /// Pointer to the graphics renderer
    IRenderer * mpRenderer;

    /// Flag if the game is game loop is running
    bool mIsGameRunning;

    /// Flag if the game is running slowly (needing multiple updates to catch up)
    bool mIsRunningSlowly;

    /// Windows timer frequency (number of ticks per second)
    TimeT mTimerFrequency;

    /// Number of times to run the update method per second
    TimeT mUpdateFrequency;

    /// Estimated maximum skew for the internal windows sleep command
    TimeT mMaximumSleepSkew;
};

#endif