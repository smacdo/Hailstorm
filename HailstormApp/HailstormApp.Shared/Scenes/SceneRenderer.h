#pragma once

#include "Common/DeviceResources.h"
#include "Rendering/ShaderStructures.h"
#include "Common/StepTimer.h"

#include <memory>
#include <concrt.h>

// Forward declarations.
namespace Hailstorm
{
    class StepTimer;
}

namespace HailstormApp
{
	// Base class for 3d scene demos.
	class SceneRenderer
	{
	public:
        // Loads vertex and pixel shaders from files and instantiates the cube geometry.
        SceneRenderer();
        virtual ~SceneRenderer();

        // Initializes view parameters when the window size changes.
		virtual void CreateDeviceDependentResources();
		virtual void CreateWindowSizeDependentResources();
		virtual void ReleaseDeviceDependentResources();
		virtual void Update(const Hailstorm::StepTimer& timer);
		virtual void Render() = 0;
		virtual void StartTracking();
		virtual void TrackingUpdate(float positionX);
		virtual void StopTracking();
		virtual bool IsTracking() const { return mTracking; }

    protected:
        virtual void OnTrackingUpdate(float positionX);

    private:
        bool mTracking;
	};
}

