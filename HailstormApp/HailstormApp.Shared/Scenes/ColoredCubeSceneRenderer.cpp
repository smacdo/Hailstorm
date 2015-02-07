#include "pch.h"
#include "ColoredCubeSceneRenderer.h"
#include "Common/DirectXHelper.h"

using namespace HailstormApp;
using namespace Hailstorm;

using namespace DirectX;
using namespace Windows::Foundation;

/**
* Loads vertex and pixel shaders from files and instantiates the cube geometry.
*/
ColoredCubeSceneRenderer::ColoredCubeSceneRenderer(std::shared_ptr<Hailstorm::DeviceResources> deviceResources)
    : SceneRenderer(),
      mLoadingComplete(false),
      mDegreesPerSecond(45),
      mIndexCount(0),
      mDeviceResources(deviceResources)
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

/**
 * Destructor.
 */
ColoredCubeSceneRenderer::~ColoredCubeSceneRenderer()
{
}

/**
* Initializes view parameters when the window size changes.
*/
void ColoredCubeSceneRenderer::CreateWindowSizeDependentResources()
{
    Size outputSize = mDeviceResources->GetOutputSize();
    float aspectRatio = outputSize.Width / outputSize.Height;
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // Note that the OrientationTransform3D matrix is post-multiplied here in order to correctly orient the scene to match
    // the display orientation. This post-multiplication step is required for any draw calls that are made to the swap
    // chain render target. For draw calls to other targets, this transform should not be applied.

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
        fovAngleY,          // Top down field of view angle in radians.
        aspectRatio,        // Aspect ratio of view space (X:Y).
        0.01f,              // Distance to the near clipping plane.
        100.0f);            // Distance to the far clipping plane.

    XMFLOAT4X4 orientation = mDeviceResources->GetOrientationTransform3D();
    XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

    XMStoreFloat4x4(
        &mConstantBufferData.projection,
        XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

    // Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
    static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
    static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
    static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

    XMStoreFloat4x4(&mConstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void ColoredCubeSceneRenderer::Update(const StepTimer& timer)
{
    if (!IsTracking())
    {
        // Convert degrees to radians, then convert seconds to rotation angle
        float radiansPerSecond = XMConvertToRadians(mDegreesPerSecond);
        double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
        float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

        Rotate(radians);
    }
}

// Rotate the 3D cube model a set amount of radians.
void ColoredCubeSceneRenderer::Rotate(float radians)
{
    // Prepare to pass the updated model matrix to the shader
    XMStoreFloat4x4(&mConstantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

// Renders one frame using the vertex and pixel shaders.
void ColoredCubeSceneRenderer::Render()
{
    // Loading is asynchronous. Only draw geometry after it's loaded.
    if (!mLoadingComplete)
    {
        return;
    }

    auto context = mDeviceResources->GetD3DDeviceContext();

    // Prepare the constant buffer to send it to the graphics device.
    context->UpdateSubresource(
        mConstantBuffer.Get(),          // Destination resource to update.
        0,                              // Index of sub-resource to update.
        nullptr,                        // Update the entire sub-resource.
        &mConstantBufferData,           // Pointer to the source data in memory.
        0,                              // Size of one row of the source data.
        0);                             // Size of one depth slice of the source data.

    // Each vertex is one instance of the VertexPositionColor struct.
    unsigned int stride = sizeof(VertexPositionColor);
    unsigned int offset = 0u;

    context->IASetVertexBuffers(
        0,                              // Input slot used while binding.
        1,                              // Only one vertex buffer in the provided array.
        mVertexBuffer.GetAddressOf(),   // Pointer to an array of vertex buffers.
        &stride,                        // Pointer to an array of stride values.
        &offset);                       // Pointer to an array of offset values.

    context->IASetIndexBuffer(
        mIndexBuffer.Get(),             // Pointer to index buffer holding index values.
        DXGI_FORMAT_R16_UINT,           // Each index is one 16-bit unsigned integer (short).
        0);                             // Zero offset from the start of the index buffer.

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(mInputLayout.Get());

    // Attach our vertex shader.
    context->VSSetShader(
        mVertexShader.Get(),            // Pointer to vertex shader to activate.
        nullptr,                        // No shader class instances provided.
        0);                             // No shader class instances provided.

    // Send the constant buffer to the graphics device.
    context->VSSetConstantBuffers(
        0,                              // No offset from start of the vertex buffer.
        1,                              // Binding one buffer.
        mConstantBuffer.GetAddressOf());// Pointer to the index buffer.

    // Attach our pixel shader.
    context->PSSetShader(
        mPixelShader.Get(),             // Pointer to the pixel shader.
        nullptr,                        // No shader class instances provided.
        0);                             // No shader class instances provided.

    // Draw the objects.
    context->DrawIndexed(
        mIndexCount,                    // Number of vertex indices to draw.
        0,                              // Start at the first (0th) entry in the index buffer.
        0);                             // Don't apply an offset to each vertex.
}

void ColoredCubeSceneRenderer::CreateDeviceDependentResources()
{
    // Load shaders asynchronously.
    auto loadVSTask = DX::ReadDataAsync(L"ColoredCubeVertexShader.cso");
    auto loadPSTask = DX::ReadDataAsync(L"ColoredCubePixelShader.cso");

    // After the vertex shader file is loaded, create the shader and input layout.
    auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateVertexShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &mVertexShader));

        static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateInputLayout(
            vertexDesc,
            ARRAYSIZE(vertexDesc),
            &fileData[0],
            fileData.size(),
            &mInputLayout));
    });

    // After the pixel shader file is loaded, create the shader and constant buffer.
    auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreatePixelShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &mPixelShader));

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateBuffer(
            &constantBufferDesc,
            nullptr,
            &mConstantBuffer));
    });

    // Once both shaders are loaded, create the mesh.
    auto createCubeTask = (createPSTask && createVSTask).then([this]() {

        // Load mesh vertices. Each vertex has a position and a color.
        static const VertexPositionColor cubeVertices[] =
        {
            { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
            { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
            { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
            { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
            { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        };

        D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
        vertexBufferData.pSysMem = cubeVertices;
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            &mVertexBuffer));

        // Load mesh indices. Each trio of indices represents
        // a triangle to be rendered on the screen.
        // For example: 0,2,1 means that the vertices with indexes
        // 0, 2 and 1 from the vertex buffer compose the 
        // first triangle of this mesh.
        static const unsigned short cubeIndices[] =
        {
            0, 2, 1, // -x
            1, 2, 3,

            4, 5, 6, // +x
            5, 7, 6,

            0, 1, 5, // -y
            0, 5, 4,

            2, 6, 7, // +y
            2, 7, 3,

            0, 4, 6, // -z
            0, 6, 2,

            1, 3, 7, // +z
            1, 7, 5,
        };

        mIndexCount = ARRAYSIZE(cubeIndices);

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = cubeIndices;
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

        CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            &mIndexBuffer));
    });

    // Once the cube is loaded, the object is ready to be rendered.
    createCubeTask.then([this]() {
        mLoadingComplete = true;
    });
}

void ColoredCubeSceneRenderer::ReleaseDeviceDependentResources()
{
    mLoadingComplete = false;
    mVertexShader.Reset();
    mInputLayout.Reset();
    mPixelShader.Reset();
    mConstantBuffer.Reset();
    mVertexBuffer.Reset();
    mIndexBuffer.Reset();
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void ColoredCubeSceneRenderer::OnTrackingUpdate(float positionX)
{
    if (IsTracking())
    {
        float radians = XM_2PI * 2.0f * positionX / mDeviceResources->GetOutputSize().Width;
        Rotate(radians);
    }
}