#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\SceneRenderer.h"

// Renders Direct3D content on the screen.
namespace HailstormApp
{
	class HailstormAppMain
	{
	public:
		HailstormAppMain();
		void CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void Update();
		bool Render();

		void OnWindowSizeChanged();
		void OnSuspending();
		void OnResuming();
		void OnDeviceRemoved();

	private:
		std::unique_ptr<SceneRenderer> m_sceneRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}