#include "OpenGLRuntimeRenderDriver.h"

#include "EditorRuntimeController.h"
#include "RenderBackendFactory.h"
#include "RenderState.h"
#include "RenderView.h"
#include "RenderViewFactory.h"
#include "RuntimeRenderFrame.h"

#include <cassert>

namespace ve::engine
{
	OpenGLRuntimeRenderDriver::~OpenGLRuntimeRenderDriver() = default;

	EngineStartupResult OpenGLRuntimeRenderDriver::Initialize(Window&,
		const ve::assets::AssetPaths& assets,
		const EngineCreateInfo&)
	{
		backend_ = ve::rendering::RenderBackendFactory::Create(
			ve::rendering::GraphicsApi::OpenGLCompatibility);
		if (!backend_) return EngineStartupResult::Failure(
			EngineStartupFailure::RenderBackendUnavailable, "Render backend factory returned no backend");
		ve::rendering::ConfigureWorldDefaults();
		view_ = RenderViewFactory::Create({ backend_->Api(), &assets, nullptr });
		if (!view_) return EngineStartupResult::Failure(
			EngineStartupFailure::RenderViewCreationFailed, "Render view creation failed");
		return EngineStartupResult::Success();
	}

	ve::rendering::RenderBackend& OpenGLRuntimeRenderDriver::Backend() noexcept
	{
		assert(backend_ != nullptr);
		return *backend_;
	}

	ve::rendering::RenderBackendCapabilities OpenGLRuntimeRenderDriver::Capabilities() const noexcept
	{
		return backend_ ? backend_->Capabilities() : ve::rendering::RenderBackendCapabilities{};
	}

	ve::blocks::BlockRegistry::TextureLoading OpenGLRuntimeRenderDriver::TextureLoading() const noexcept
	{
		return ve::blocks::BlockRegistry::TextureLoading::LoadTextures;
	}

	void OpenGLRuntimeRenderDriver::ConfigureModel(RuntimeRenderFrame& frame, const EngineCreateInfo&)
	{
		(void)frame.editor.Initialize(frame.window, frame.settings);
	}

	void OpenGLRuntimeRenderDriver::Shutdown() noexcept
	{
		if (view_) view_->ReleaseCachedResources();
		view_.reset();
		backend_.reset();
	}
}
