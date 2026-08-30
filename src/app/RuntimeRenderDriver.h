#pragma once

#include "BlockRegistry.h"
#include "EngineStartupResult.h"
#include "RenderApi.h"

#include <memory>

namespace ve::assets { struct AssetPaths; }
namespace ve::rendering { class RenderBackend; }

namespace ve::engine
{
	struct EngineCreateInfo;
	struct RuntimeRenderFrame;
	class GameModel;
	class RuntimeRenderHost;
	class Window;

	/** Owns one graphics backend and its complete runtime frame integration. */
	class RuntimeRenderDriver
	{
	public:
		virtual ~RuntimeRenderDriver() = default;
		virtual EngineStartupResult Initialize(Window& window,
			const ve::assets::AssetPaths& assets,
			const EngineCreateInfo& create_info) = 0;
		[[nodiscard]] virtual ve::rendering::RenderBackend& Backend() noexcept = 0;
		[[nodiscard]] virtual ve::rendering::RenderBackendCapabilities Capabilities() const noexcept = 0;
		[[nodiscard]] virtual ve::blocks::BlockRegistry::TextureLoading TextureLoading() const noexcept = 0;
		virtual void ConfigureModel(RuntimeRenderFrame& frame, const EngineCreateInfo& create_info) = 0;
		virtual void BeginFrame(RuntimeRenderFrame& frame, RuntimeRenderHost& host) = 0;
		virtual void UpdateGameplay(RuntimeRenderFrame& frame) = 0;
		[[nodiscard]] virtual bool DrawFrame(RuntimeRenderFrame& frame, RuntimeRenderHost& host) = 0;
		virtual void EndFrame(RuntimeRenderFrame& frame) = 0;
		virtual void Shutdown() noexcept = 0;
	};
}
