#pragma once

#include "EngineLogSettings.h"
#include "RenderApi.h"
#include "WindowCreateInfo.h"

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace ve::engine
{
	/** Startup data needed by the generic window, asset, logging, and frame host. */
	struct RuntimeHostConfiguration
	{
		WindowCreateInfo window{};
		ve::rendering::RenderBackendConfiguration render_backend{};
		RuntimeLogSettings logging{};
		bool vsync = false;
		std::vector<std::filesystem::path> asset_search_roots;
		std::function<void(const std::string&)> on_log;
	};
}
