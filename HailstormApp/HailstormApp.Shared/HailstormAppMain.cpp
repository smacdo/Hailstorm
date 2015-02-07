#include "pch.h"
#include "HailstormAppMain.h"
#include "Common\DirectXHelper.h"
#include "Common/StepTimer.h"
#include "UserInterface/GameHudRenderer.h"
#include "Scenes/LandscapeDemoSceneRenderer.h"

using namespace HailstormApp;
using namespace Hailstorm;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
HailstormAppMain::HailstormAppMain(const std::shared_ptr<DeviceResources>& deviceResources)
    : mDeviceResources(deviceResources),
      mPointerLocationX(0.0f)
{
	// Register to be notified if the Device is lost or recreated
	mDeviceResources->RegisterDeviceNotify(this);

    // Set our update loop to be a fixed 60 times/second.
    mTimer.SetFixedTimeStep(true);
    mTimer.SetTargetElapsedSeconds(1.0 / 60);

    // Create the 2d user interface and a 3d demo scene.
    mSceneRenderer = std::unique_ptr<SceneRenderer>(new LandscapeDemoSceneRenderer(mDeviceResources));
    mGameHudRenderer = std::unique_ptr<GameHudRenderer>(new GameHudRenderer(mDeviceResources));
}

HailstormAppMain::~HailstormAppMain()
{
	// Deregister device notification
	mDeviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void HailstormAppMain::CreateWindowSizeDependentResources() 
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	mSceneRenderer->CreateWindowSizeDependentResources();
}

void HailstormAppMain::StartRenderLoop()
{
	// If the animation render loop is already running then do not start another thread.
	if (mRenderLoopWorker != nullptr && mRenderLoopWorker->Status == AsyncStatus::Started)
	{
		return;
	}

	// Create a task that will be run on a background thread.
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		// Calculate the updated frame and render once per vertical blanking interval.
		while (action->Status == AsyncStatus::Started)
		{
			critical_section::scoped_lock lock(mCriticalSection);

			Update();

			if (Render())
			{
				mDeviceResources->Present();
			}
		}
	});

	// Run task on a dedicated high priority background thread.
	mRenderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void HailstormAppMain::StopRenderLoop()
{
	mRenderLoopWorker->Cancel();
}

// Updates the application state once per frame.
void HailstormAppMain::Update() 
{
	ProcessInput();

	// Update scene objects.
	mTimer.Update([&](const StepTimer& timer)
	{
		mSceneRenderer->Update(mTimer);
        mGameHudRenderer->Update(mTimer);
	});
}

// Process all input from the user before updating game state
void HailstormAppMain::ProcessInput()
{
	mSceneRenderer->TrackingUpdate(mPointerLocationX);
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool HailstormAppMain::Render() 
{
	// Don't try to render anything before the first Update.
	if (mTimer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = mDeviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = mDeviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { mDeviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, mDeviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(mDeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(mDeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render the scene objects.
	mSceneRenderer->Render();
	mGameHudRenderer->Render();

	return true;
}

// Notifies renderers that device resources need to be released.
void HailstormAppMain::OnDeviceLost()
{
	mSceneRenderer->ReleaseDeviceDependentResources();
    mGameHudRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void HailstormAppMain::OnDeviceRestored()
{
    mSceneRenderer->CreateDeviceDependentResources();
    mGameHudRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void HailstormAppMain::StartTracking()
{
    mSceneRenderer->StartTracking();
}

void HailstormAppMain::TrackingUpdate(float positionX)
{
    mPointerLocationX = positionX;
}

void HailstormAppMain::StopTracking()
{
    mSceneRenderer->StopTracking();
}

bool HailstormAppMain::IsTracking() const
{
    return mSceneRenderer->IsTracking();
}

Concurrency::critical_section& HailstormAppMain::GetCriticalSection()
{
    return mCriticalSection;
}
