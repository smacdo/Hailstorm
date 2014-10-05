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
#ifndef SCOTT_GAME_CLIENT_H
#define SCOTT_GAME_CLIENT_H

#include "runtime/gametime.h"
#include <memory>               // Smart pointer.

class IWindow;
class DXRenderer;
class DemoScene;

/**
 * This is the foundation class for a hailstorm game client. A custom game
 * shall inherit from this class to provide custom game play and functionality
 */
class GameClient
{
public:
    GameClient(std::shared_ptr<IWindow> window, DXRenderer *pRenderer);
    virtual ~GameClient();

    void Run(DemoScene *pDemoScene);
    void SetUpdateFrequency(int numUpdatesPerSecond);

    bool IsGameRunning() const { return mIsGameRunning; }
    bool IsRunningSlowly() const { return mIsRunningSlowly; }

protected:
    virtual void Initialize();
    virtual void LoadContent();
    virtual void UnloadContent();
    virtual void Update(TimeT simulationTime, TimeT deltaTime);
    virtual void Draw(TimeT simulationTime, double interpolation);

private:
    void RunMainGameLoop();
    TimeT GetCurrentTime() const;
    TimeT CalculateSystemTimerFrequency();
    void InitializeClient();

private:
    /// Pointer to the main rendering window
    std::shared_ptr<IWindow> mWindow;

    /// Pointer to the graphics renderer
    std::shared_ptr<DXRenderer> mRenderer;

    /// Demo scene to show.
    std::unique_ptr<DemoScene> mDemoScene;

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