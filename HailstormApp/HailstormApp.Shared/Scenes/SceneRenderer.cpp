#include "pch.h"
#include "Scenes/SceneRenderer.h"
#include "Common/DirectXHelper.h"

using namespace HailstormApp;
using namespace Hailstorm;

using namespace DirectX;
using namespace Windows::Foundation;

/**
 * Loads vertex and pixel shaders from files and instantiates the cube geometry.
 */
SceneRenderer::SceneRenderer()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

SceneRenderer::~SceneRenderer()
{
}

/**
 * Initializes view parameters when the window size changes.
 */
void SceneRenderer::CreateWindowSizeDependentResources()
{
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void SceneRenderer::Update(const StepTimer& timer)
{
}

void SceneRenderer::StartTracking()
{
	mTracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void SceneRenderer::TrackingUpdate(float positionX)
{
    OnTrackingUpdate(positionX);
}

void SceneRenderer::OnTrackingUpdate(float positionX)
{

}

void SceneRenderer::StopTracking()
{
	mTracking = false;
}

void SceneRenderer::CreateDeviceDependentResources()
{
}

void SceneRenderer::ReleaseDeviceDependentResources()
{
}