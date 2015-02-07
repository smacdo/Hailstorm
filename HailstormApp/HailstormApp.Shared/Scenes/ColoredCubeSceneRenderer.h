#pragma once
#include "SceneRenderer.h"

// Forward declarations.
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

namespace Hailstorm
{
    class StepTimer;
}

namespace HailstormApp
{
    class ColoredCubeSceneRenderer : public SceneRenderer
    {
    public:
        ColoredCubeSceneRenderer(std::shared_ptr<Hailstorm::DeviceResources> deviceResources);
        virtual ~ColoredCubeSceneRenderer() override;

        // Initializes view parameters when the window size changes.
        virtual void CreateDeviceDependentResources() override;
        virtual void CreateWindowSizeDependentResources() override;
        virtual void ReleaseDeviceDependentResources() override;
        virtual void Update(const Hailstorm::StepTimer& timer) override;
        virtual void Render() override;

    protected:
        virtual void OnTrackingUpdate(float positionX) override;

    private:
        void Rotate(float radians);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<Hailstorm::DeviceResources> mDeviceResources;

        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>	mInputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		mVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		mIndexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>	mVertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>	mPixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		mConstantBuffer;

        // System resources for cube geometry.
        ModelViewProjectionConstantBuffer mConstantBufferData;
        uint32 mIndexCount;

        // Variables used with the rendering loop.
        bool mLoadingComplete;
        float mDegreesPerSecond;
    };

}
