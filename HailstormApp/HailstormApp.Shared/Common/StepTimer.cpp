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
#pragma once
#include "pch.h"
#include "StepTimer.h"

#include <wrl.h>
#include <functional>

using namespace Hailstorm;

StepTimer::StepTimer()
    : mElapsedTicks(0),
      mTotalTicks(0),
      mLeftOverTicks(0),
      mFrameCount(0),
      mFramesPerSecond(0),
      mFramesThisSecond(0),
      mQpcSecondCounter(0),
      mIsFixedTimeStep(false),
      mTargetElapsedTicks(TicksPerSecond / 60)
{
    if (!QueryPerformanceFrequency(&mQpcFrequency))
    {
        throw ref new Platform::FailureException();
    }

    if (!QueryPerformanceCounter(&mQpcLastTime))
    {
        throw ref new Platform::FailureException();
    }

    // Initialize max delta to 1/10 of a second.
    mQpcMaxDelta = mQpcFrequency.QuadPart / 10;
}

// Get elapsed time since the previous Update call.
uint64 StepTimer::GetElapsedTicks() const
{
    return mElapsedTicks;
}

double StepTimer::GetElapsedSeconds() const
{
    return TicksToSeconds(mElapsedTicks);
}

// Get total time since the start of the program.
uint64 StepTimer::GetTotalTicks() const
{
    return mTotalTicks;
}

double StepTimer::GetTotalSeconds() const
{
    return TicksToSeconds(mTotalTicks);
}

// Get total number of updates since start of the program.
uint32 StepTimer::GetFrameCount() const
{
    return mFrameCount;
}

// Get the current framerate.
uint32 StepTimer::GetFramesPerSecond() const
{
    return mFramesPerSecond;
}

// Set whether to use fixed or variable timestep mode.
void StepTimer::SetFixedTimeStep(bool isFixedTimestep)
{
    mIsFixedTimeStep = isFixedTimestep;
}

// Set how often to call Update when in fixed timestep mode.
void StepTimer::SetTargetElapsedTicks(uint64 targetElapsed)
{
    mTargetElapsedTicks = targetElapsed;
}

void StepTimer::SetTargetElapsedSeconds(double targetElapsed)
{
    mTargetElapsedTicks = SecondsToTicks(targetElapsed);
}

double StepTimer::TicksToSeconds(uint64 ticks)
{
    return static_cast<double>(ticks) / TicksPerSecond;
}

uint64 StepTimer::SecondsToTicks(double seconds)
{
    return static_cast<uint64>(seconds * TicksPerSecond);
}

void StepTimer::ResetElapsedTime()
{
    if (!QueryPerformanceCounter(&mQpcLastTime))
    {
        throw ref new Platform::FailureException();
    }

    mLeftOverTicks = 0;
    mFramesPerSecond = 0;
    mFramesThisSecond = 0;
    mQpcSecondCounter = 0;
}

// Update timer state, calling the specified Update function the appropriate number of times.
void StepTimer::Update(std::function<void(const StepTimer&)> logic)
{
    // Query the current time.
    LARGE_INTEGER currentTime;

    if (!QueryPerformanceCounter(&currentTime))
    {
        throw ref new Platform::FailureException();
    }

    uint64 timeDelta = currentTime.QuadPart - mQpcLastTime.QuadPart;

    mQpcLastTime = currentTime;
    mQpcSecondCounter += timeDelta;

    // Clamp excessively large time deltas (e.g. after paused in the debugger).
    if (timeDelta > mQpcMaxDelta)
    {
        timeDelta = mQpcMaxDelta;
    }

    // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
    timeDelta *= TicksPerSecond;
    timeDelta /= mQpcFrequency.QuadPart;

    uint32 lastFrameCount = mFrameCount;

    if (mIsFixedTimeStep)
    {
        // Fixed timestep update logic

        // If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
        // the clock to exactly match the target value. This prevents tiny and irrelevant errors
        // from accumulating over time. Without this clamping, a game that requested a 60 fps
        // fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
        // accumulate enough tiny errors that it would drop a frame. It is better to just round 
        // small deviations down to zero to leave things running smoothly.

        if (abs(static_cast<int64>(timeDelta - mTargetElapsedTicks)) < TicksPerSecond / 4000)
        {
            timeDelta = mTargetElapsedTicks;
        }

        mLeftOverTicks += timeDelta;

        while (mLeftOverTicks >= mTargetElapsedTicks)
        {
            mElapsedTicks = mTargetElapsedTicks;
            mTotalTicks += mTargetElapsedTicks;
            mLeftOverTicks -= mTargetElapsedTicks;
            mFrameCount++;

            logic(*this);
        }
    }
    else
    {
        // Variable timestep update logic.
        mElapsedTicks = timeDelta;
        mTotalTicks += timeDelta;
        mLeftOverTicks = 0;
        mFrameCount++;

        logic(*this);
    }

    // Track the current framerate.
    if (mFrameCount != lastFrameCount)
    {
        mFramesThisSecond++;
    }

    if (mQpcSecondCounter >= static_cast<uint64>(mQpcFrequency.QuadPart))
    {
        mFramesPerSecond = mFramesThisSecond;
        mFramesThisSecond = 0;
        mQpcSecondCounter %= mQpcFrequency.QuadPart;
    }
}
