#include "DemoCommandLine.h"

#include <charconv>

namespace voxel_demo
{
	namespace
	{
		[[nodiscard]] bool TryParsePositiveInteger(std::string_view text, int& value) noexcept
		{
			constexpr int kMaximumFrameLimit = 100000;
			int parsed_value = 0;
			const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), parsed_value);
			if (error != std::errc{} || end != text.data() + text.size() ||
				parsed_value <= 0 || parsed_value > kMaximumFrameLimit) return false;
			value = parsed_value;
			return true;
		}

		[[nodiscard]] bool TryParseGraphicsApi(
			std::string_view text,
			voxel::GraphicsApi& graphics_api) noexcept
		{
			if (text == "vulkan")
			{
				graphics_api = voxel::GraphicsApi::Vulkan;
				return true;
			}
			if (text == "opengl")
			{
				graphics_api = voxel::GraphicsApi::OpenGLCompatibility;
				return true;
			}
			return false;
		}

		[[nodiscard]] bool RuntimeLayoutOptionsAreCompatible(const DemoOptions& options) noexcept
		{
			const bool has_assets = !options.asset_directory.empty();
			const bool has_shaders = !options.vulkan_shader_directory.empty();
			if (!has_assets && !has_shaders) return true;
			if (!has_assets) return false;
			return options.graphics_api != voxel::GraphicsApi::Vulkan || has_shaders;
		}
	}

	DemoOptions ParseOptions(std::span<const std::string_view> arguments) noexcept
	{
		DemoOptions options{};
		for (std::size_t index = 1; index < arguments.size(); ++index)
		{
			if (index + 1 >= arguments.size())
			{
				options.valid = false;
				return options;
			}
			const std::string_view option = arguments[index];
			const std::string_view value = arguments[++index];
			bool parsed = false;
			if (option == "--smoke-frames")
				parsed = TryParsePositiveInteger(value, options.smoke_frame_limit);
			else if (option == "--graphics-api")
				parsed = TryParseGraphicsApi(value, options.graphics_api);
			else if (option == "--asset-directory")
			{
				options.asset_directory = value;
				parsed = !value.empty();
			}
			else if (option == "--vulkan-shader-directory")
			{
				options.vulkan_shader_directory = value;
				parsed = !value.empty();
			}
			if (!parsed)
			{
				options.valid = false;
				return options;
			}
		}
		options.valid = RuntimeLayoutOptionsAreCompatible(options);
		return options;
	}
}
