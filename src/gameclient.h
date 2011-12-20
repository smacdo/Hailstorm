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

protected:
    virtual bool initialize();
    virtual void loadContent();
    virtual void unloadContent();
    virtual void update( double currentTime );
    virtual void draw( double currentTime );

private:
    bool initializeEngine();

private:
    MainWindow * mpMainWindow;
    IRenderer * mpRenderer;
    bool mKeepRunning;
};

#endif