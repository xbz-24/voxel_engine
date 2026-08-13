#pragma once

#include "RenderView.h"
#include "RuntimeRenderDriver.h"
#include "VulkanBackend.h"
#include "VulkanFrameOrchestrator.h"

#include <memory>

namespace ve::engine
{
	class VulkanRuntimeRenderDriver final : public RuntimeRenderDriver
	{
	public:
		~VulkanRuntimeRenderDriver() override;
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
		[[nodiscard]] ve::rendering::VulkanFrameInput CaptureInput(RuntimeRenderFrame& frame);
		std::unique_ptr<ve::rendering::VulkanBackend> backend_;
		std::unique_ptr<RenderView> view_;
		ve::rendering::VulkanFrameOrchestrator orchestrator_;
		ve::rendering::VulkanOverlaySettings overlay_settings_;
	};
}
