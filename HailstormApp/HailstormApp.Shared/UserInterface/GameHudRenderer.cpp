#include "pch.h"
#include "UserInterface/GameHudRenderer.h"
#include "Common/DirectXHelper.h"

using namespace HailstormApp;
using namespace Hailstorm;
using namespace D2D1;
using namespace DX;

/**
 * \brief Initialize and configure Direct2d resources used for the user interface.
 */
GameHudRenderer::GameHudRenderer(std::shared_ptr<DeviceResources> deviceResources)
    : mText(L""),
	  mDeviceResources(deviceResources)
{
	// Create device independent resources
	ThrowIfFailed(
		mDeviceResources->GetDWriteFactory()->CreateTextFormat(
			L"Segoe UI",
			nullptr,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			32.0f,
			L"en-US",
			&mTextFormat));

	DX::ThrowIfFailed(mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
	DX::ThrowIfFailed(mDeviceResources->GetD2DFactory()->CreateDrawingStateBlock(&mStateBlock));

	CreateDeviceDependentResources();
}

void GameHudRenderer::Update(const StepTimer& timer)
{
	// Update display text.
	uint32 fps = timer.GetFramesPerSecond();

	mText = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

	DX::ThrowIfFailed(
		mDeviceResources->GetDWriteFactory()->CreateTextLayout(
			mText.c_str(),
			(uint32) mText.length(),
			mTextFormat.Get(),
			240.0f, // Max width of the input text.
			50.0f, // Max height of the input text.
			&mTextLayout
			)
		);
}

void GameHudRenderer::Render()
{
	ID2D1DeviceContext* context = mDeviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = mDeviceResources->GetLogicalSize();

	context->SaveDrawingState(mStateBlock.Get());
	context->BeginDraw();

	// Position on the bottom right corner
    DWRITE_TEXT_METRICS	textMetrics = { 0 };
    DX::ThrowIfFailed(mTextLayout->GetMetrics(&textMetrics));

	D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
        logicalSize.Width - textMetrics.layoutWidth,
        logicalSize.Height - textMetrics.height);

	context->SetTransform(screenTranslation * mDeviceResources->GetOrientationTransform2D());

	DX::ThrowIfFailed(mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));

	context->DrawTextLayout(
		D2D1::Point2F(0.f, 0.f),
		mTextLayout.Get(),
		mWhiteBrush.Get());

	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device is lost. It will be handled during the
    // next call to Present.
	HRESULT hr = context->EndDraw();

	if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}

	context->RestoreDrawingState(mStateBlock.Get());
}

void GameHudRenderer::CreateDeviceDependentResources()
{
    DX::ThrowIfFailed(
        mDeviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &mWhiteBrush));
}

void GameHudRenderer::ReleaseDeviceDependentResources()
{
	mWhiteBrush.Reset();
}