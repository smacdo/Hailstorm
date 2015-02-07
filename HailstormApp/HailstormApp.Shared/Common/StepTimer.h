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

#include <wrl.h>
#include <functional>

namespace Hailstorm
{
    /**
     * \brief Simple timer for games.
     */
	class StepTimer
	{
	public:
        StepTimer();

		// Get elapsed time since the previous Update call.
        uint64 GetElapsedTicks() const;
        double GetElapsedSeconds() const;

		// Get total time since the start of the program.
        uint64 GetTotalTicks() const;
        double GetTotalSeconds() const;

		// Get total number of updates since start of the program.
        uint32 GetFrameCount() const;

		// Get the current framerate.
        uint32 GetFramesPerSecond() const;

		// Set whether to use fixed or variable timestep mode.
        void SetFixedTimeStep(bool isFixedTimestep);

		// Set how often to call Update when in fixed timestep mode.
        void SetTargetElapsedTicks(uint64 targetElapsed);
        void SetTargetElapsedSeconds(double targetElapsed);

		// Integer format represents time using 10,000,000 ticks per second.
		static const uint64 TicksPerSecond = 10000000;

        static double TicksToSeconds(uint64 ticks);
        static uint64 SecondsToTicks(double seconds);

		// After an intentional timing discontinuity (for instance a blocking IO operation)
		// call this to avoid having the fixed timestep logic attempt a set of catch-up 
		// Update calls.
        void ResetElapsedTime();

		// Update timer state, calling the specified Update function the appropriate number of times.
        void Update(std::function<void(const StepTimer&)> logic);

	private:
		// Source timing data uses QPC units.
		LARGE_INTEGER mQpcFrequency;
		LARGE_INTEGER mQpcLastTime;
		uint64 mQpcMaxDelta;

		// Derived timing data uses a canonical tick format.
		uint64 mElapsedTicks;
		uint64 mTotalTicks;
		uint64 mLeftOverTicks;

		// Members for tracking the framerate.
		uint32 mFrameCount;
		uint32 mFramesPerSecond;
		uint32 mFramesThisSecond;
		uint64 mQpcSecondCounter;

		// Members for configuring fixed timestep mode.
		bool mIsFixedTimeStep;
		uint64 mTargetElapsedTicks;
	};
}
