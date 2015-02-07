#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Scenes/SceneRenderer.h"
#include "UserInterface/GameHudRenderer.h"

#include <memory>
#include <concrt.h>

// Renders Direct2D and 3D content on the screen.
namespace HailstormApp
{
    class HailstormAppMain : public Hailstorm::IDeviceNotify
	{
	public:
        HailstormAppMain(const std::shared_ptr<Hailstorm::DeviceResources>& deviceResources);
		~HailstormAppMain();
		void CreateWindowSizeDependentResources();
        void StartTracking();
        void TrackingUpdate(float positionX);
        void StopTracking();
        bool IsTracking() const;
		void StartRenderLoop();
		void StopRenderLoop();
        Concurrency::critical_section& GetCriticalSection();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		void ProcessInput();
		void Update();
		bool Render();

		// Cached pointer to device resources.
        std::shared_ptr<Hailstorm::DeviceResources> mDeviceResources;
        std::unique_ptr<SceneRenderer> mSceneRenderer;
		std::unique_ptr<GameHudRenderer> mGameHudRenderer;

		Windows::Foundation::IAsyncAction^ mRenderLoopWorker;
		Concurrency::critical_section mCriticalSection;

		// Rendering loop timer.
		Hailstorm::StepTimer mTimer;

		// Track current input pointer position.
		float mPointerLocationX;
	};
}