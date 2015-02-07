#pragma once

#include <string>
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"

#include <memory>

// Forward declarations
struct ID2D1SolidColorBrush;
struct ID2D1DrawingStateBlock;
struct IDWriteTextLayout;
struct IDWriteTextFormat;

namespace Hailstorm
{
    class StepTimer;
}

namespace HailstormApp
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class GameHudRenderer
	{
	public:
        GameHudRenderer(std::shared_ptr<Hailstorm::DeviceResources> deviceResources);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();

        // Updates the text to be displayed.
		void Update(const Hailstorm::StepTimer& timer);

        // Renders a frame to the screen.
		void Render();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<Hailstorm::DeviceResources> mDeviceResources;

		// Resources related to text rendering.
		std::wstring mText;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> mWhiteBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock> mStateBlock;
		Microsoft::WRL::ComPtr<IDWriteTextLayout> mTextLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> mTextFormat;
	};
}