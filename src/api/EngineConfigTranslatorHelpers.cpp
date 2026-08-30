#include "EngineConfigTranslatorInternal.h"

#include <utility>

namespace voxel::detail::config_translation
{
	[[nodiscard]] glm::vec3 ToInternalVec3(Vec3 value) noexcept
		{
			return { value.x, value.y, value.z };
		}

		[[nodiscard]] Vec3 ToPublicVec3(glm::vec3 value) noexcept
		{
			return { value.x, value.y, value.z };
		}

		[[nodiscard]] ve::rendering::VoxelRenderStyle ToInternalVoxelRenderStyle(
			const VoxelRenderStyle& style) noexcept
		{
			return ve::rendering::VoxelRenderStyle{
				ToInternalVec3(style.sun_direction),
				ToInternalVec3(style.sun_color),
				ToInternalVec3(style.sky_horizon_color),
				ToInternalVec3(style.sky_zenith_color),
				style.sun_intensity,
				style.exposure,
				style.fog_start_distance,
				style.fog_end_distance,
				style.fog_strength,
				style.cloud_coverage,
				style.cloud_density,
				style.cloud_speed,
				style.surface_detail_strength,
				style.water_reflection_strength,
				style.shadow_strength,
				style.specular_strength,
				style.ambient_occlusion_strength,
				style.aerial_perspective_strength,
				style.subsurface_scattering_strength,
				style.cloud_light_absorption,
				style.directional_shadow_strength,
				style.directional_shadow_softness,
				style.directional_shadow_distance
			};
		}

		[[nodiscard]] ve::rendering::GraphicsApi ToInternalApi(GraphicsApi api) noexcept
		{
			switch (api)
			{
			case GraphicsApi::OpenGLCompatibility:
				return ve::rendering::GraphicsApi::OpenGLCompatibility;
			case GraphicsApi::DirectX12:
				return ve::rendering::GraphicsApi::DirectX12;
			case GraphicsApi::Vulkan:
			default:
				return ve::rendering::GraphicsApi::Vulkan;
			}
		}

		[[nodiscard]] ve::log::Level ToInternalLogLevel(LogLevel level) noexcept
		{
			switch (level)
			{
			case LogLevel::Trace:
				return ve::log::Level::Trace;
			case LogLevel::Debug:
				return ve::log::Level::Debug;
			case LogLevel::Warning:
				return ve::log::Level::Warning;
			case LogLevel::Error:
				return ve::log::Level::Error;
			case LogLevel::Fatal:
				return ve::log::Level::Fatal;
			case LogLevel::Default:
			case LogLevel::Info:
			default:
				return ve::log::Level::Info;
			}
		}

		[[nodiscard]] ve::engine::RuntimeLogSettings ToInternalLogging(LogSettings logging)
		{
			ve::engine::RuntimeLogSettings result{};
			if (logging.minimum_level != LogLevel::Default)
			{
				result.minimum_level = ToInternalLogLevel(logging.minimum_level);
			}
			result.console_enabled = logging.console_enabled;
			result.file_output_enabled = logging.file_output_enabled;
			result.file_output_path = std::move(logging.file_output_path);
			return result;
		}
}
