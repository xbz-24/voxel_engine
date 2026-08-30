#pragma once

#include "EngineStartupResult.h"
#include "RenderApi.h"

#include <memory>

namespace ve::engine
{
	class RuntimeRenderDriver;

	class RuntimeRenderDriverFactory
	{
	public:
		[[nodiscard]] static std::unique_ptr<RuntimeRenderDriver> Create(
			ve::rendering::GraphicsApi api);
		[[nodiscard]] static EngineStartupResult MissingDriverResult(
			ve::rendering::GraphicsApi api);
	};
}
