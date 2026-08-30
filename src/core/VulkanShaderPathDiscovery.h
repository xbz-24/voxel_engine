#pragma once

#include <filesystem>
#include <optional>

namespace ve::assets
{
	/** Returns whether a directory contains the complete runtime Vulkan shader bundle. */
	[[nodiscard]] bool IsCompleteVulkanShaderDirectory(
		const std::filesystem::path& shader_directory);

	/** Resolves an explicit shader directory or discovers one above the executable. */
	[[nodiscard]] std::filesystem::path ResolveVulkanShaderDirectory(
		const std::optional<std::filesystem::path>& explicit_directory,
		const std::filesystem::path& discovery_start_directory = {});
}
