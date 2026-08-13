#pragma once

#include "RenderBackend.h"
#include "RenderView.h"
#include "RuntimeRenderDriver.h"

#include <memory>

namespace ve::engine
{
	class OpenGLRuntimeRenderDriver final : public RuntimeRenderDriver
	{
	public:
		~OpenGLRuntimeRenderDriver() override;
		EngineStartupResult Initialize(Window& window,
			const ve::assets::AssetPaths& assets,
			const EngineCreateInfo& create_info) override;
		[[nodiscard]] ve::rendering::RenderBackend& Backend() noexcept override;
		[[nodiscard]] ve::rendering::RenderBackendCapabilities Capabilities() const noexcept override;
		[[nodiscard]] ve::blocks::BlockRegistry::TextureLoading TextureLoading() const noexcept override;
		void ConfigureModel(RuntimeRenderFrame& frame, const EngineCreateInfo& create_info) override;
		void BeginFrame(RuntimeRenderFrame& frame, RuntimeRenderHost& host) override;
		void UpdateGameplay(RuntimeRenderFrame& frame) override;
		[[nodiscard]] bool DrawFrame(RuntimeRenderFrame& frame, RuntimeRenderHost& host) override;
		void EndFrame(RuntimeRenderFrame& frame) override;
		void Shutdown() noexcept override;

	private:
		std::unique_ptr<ve::rendering::RenderBackend> backend_;
		std::unique_ptr<RenderView> view_;
	};
}
