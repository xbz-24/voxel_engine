#include "EngineConfiguration.h"

#include <algorithm>
#include <cmath>

namespace ve::engine
{
	namespace
	{
		[[nodiscard]] bool IsKnownGraphicsApi(ve::rendering::GraphicsApi api) noexcept
		{
			switch (api)
			{
			case ve::rendering::GraphicsApi::Vulkan:
			case ve::rendering::GraphicsApi::OpenGLCompatibility:
			case ve::rendering::GraphicsApi::DirectX12:
				return true;
			default:
				return false;
			}
		}

		[[nodiscard]] bool IsKnownRenderBackendSelectionPolicy(
			ve::rendering::RenderBackendSelectionPolicy policy) noexcept
		{
			switch (policy)
			{
			case ve::rendering::RenderBackendSelectionPolicy::ExactBackendOnly:
			case ve::rendering::RenderBackendSelectionPolicy::BestAvailable:
			case ve::rendering::RenderBackendSelectionPolicy::Headless:
				return true;
			default:
				return false;
			}
		}

		[[nodiscard]] bool IsKnownVulkanDemoPreset(
			ve::rendering::VulkanMinecraftDemoPreset preset) noexcept
		{
			switch (preset)
			{
			case ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert:
			case ve::rendering::VulkanMinecraftDemoPreset::AquaModel:
			case ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium:
				return true;
			default:
				return false;
			}
		}

		[[nodiscard]] bool IsKnownWorldBlockEditKind(ve::world::WorldBlockEdit::Kind kind) noexcept
		{
			switch (kind)
			{
			case ve::world::WorldBlockEdit::Kind::SetBlock:
			case ve::world::WorldBlockEdit::Kind::FillBox:
				return true;
			default:
				return false;
			}
		}

		[[nodiscard]] bool IsFinite(glm::vec3 value) noexcept
		{
			return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
		}

		[[nodiscard]] bool IsFiniteNonNegative(glm::vec3 value) noexcept
		{
			return IsFinite(value) && value.x >= 0.0f && value.y >= 0.0f && value.z >= 0.0f;
		}

		[[nodiscard]] bool IsFiniteInRange(float value, float minimum, float maximum) noexcept
		{
			return std::isfinite(value) && value >= minimum && value <= maximum;
		}

		void ValidateVoxelRenderStyle(
			const ve::rendering::VoxelRenderStyle& style,
			std::vector<std::string>& issues)
		{
			const float largest_sun_direction_component = std::max(
				std::abs(style.sun_direction.x),
				std::max(std::abs(style.sun_direction.y), std::abs(style.sun_direction.z)));
			if (!IsFinite(style.sun_direction) || largest_sun_direction_component <= 0.000001f)
			{
				issues.push_back("voxel_render_style.sun_direction must be finite and non-zero");
			}
			if (!IsFiniteNonNegative(style.sun_color) ||
				!IsFiniteNonNegative(style.sky_horizon_color) ||
				!IsFiniteNonNegative(style.sky_zenith_color))
			{
				issues.push_back("voxel_render_style colors must contain finite non-negative values");
			}
			if (!std::isfinite(style.sun_intensity) || style.sun_intensity < 0.0f)
			{
				issues.push_back("voxel_render_style.sun_intensity must be finite and non-negative");
			}
			if (!std::isfinite(style.exposure) || style.exposure <= 0.0f)
			{
				issues.push_back("voxel_render_style.exposure must be finite and greater than zero");
			}
			if (!std::isfinite(style.fog_start_distance) || style.fog_start_distance < 0.0f)
			{
				issues.push_back("voxel_render_style.fog_start_distance must be finite and non-negative");
			}
			if (!std::isfinite(style.fog_end_distance) || style.fog_end_distance <= style.fog_start_distance)
			{
				issues.push_back("voxel_render_style.fog_end_distance must be finite and greater than fog_start_distance");
			}
			if (!IsFiniteInRange(style.fog_strength, 0.0f, 1.0f) ||
				!IsFiniteInRange(style.cloud_coverage, 0.0f, 1.0f) ||
				!IsFiniteInRange(style.cloud_density, 0.0f, 1.0f))
			{
				issues.push_back("voxel_render_style atmosphere strengths must be finite and between 0 and 1");
			}
			if (!std::isfinite(style.cloud_speed) || style.cloud_speed < 0.0f)
			{
				issues.push_back("voxel_render_style.cloud_speed must be finite and non-negative");
			}
			if (!IsFiniteInRange(style.surface_detail_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.water_reflection_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.shadow_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.specular_strength, 0.0f, 2.0f))
			{
				issues.push_back("voxel_render_style surface strengths must be finite and between 0 and 2");
			}
		}
	}

	std::vector<std::string> ValidateEngineCreateInfo(const EngineCreateInfo& create_info)
	{
		std::vector<std::string> issues;
		if (create_info.window.width <= 0)
		{
			issues.push_back("window.width must be greater than zero");
		}
		if (create_info.window.height <= 0)
		{
			issues.push_back("window.height must be greater than zero");
		}
		if (create_info.window.display_index < 0)
		{
			issues.push_back("window.display_index must be zero or greater");
		}
		if (create_info.window.refresh_rate_hertz < 0)
		{
			issues.push_back("window.refresh_rate_hertz must be zero or greater");
		}
		if (!IsKnownGraphicsApi(create_info.render_backend.preferred_api))
		{
			issues.push_back("render_backend.preferred_api is not a known graphics api");
		}
		if (!IsKnownRenderBackendSelectionPolicy(create_info.render_backend.selection_policy))
		{
			issues.push_back("render_backend.selection_policy is not a known selection policy");
		}
		if (!IsKnownVulkanDemoPreset(create_info.vulkan_demo_preset))
		{
			issues.push_back("vulkan_demo_preset is not a known demo preset");
		}
		ValidateVoxelRenderStyle(create_info.voxel_render_style, issues);
		if (create_info.world_size_chunks <= 0)
		{
			issues.push_back("world_size_chunks must be greater than zero");
		}
		if (create_info.render_distance_chunks < 0)
		{
			issues.push_back("render_distance_chunks must be zero or greater");
		}
		if (create_info.has_custom_camera)
		{
			if (!IsFinite(create_info.camera_position))
			{
				issues.push_back("camera_position must contain finite values");
			}
			if (!IsFinite(create_info.camera_look_at))
			{
				issues.push_back("camera_look_at must contain finite values");
			}
		}
		for (const WorldBlockEdit& world_edit : create_info.world_edits)
		{
			if (!IsKnownWorldBlockEditKind(world_edit.kind))
			{
				issues.push_back("world_edits contains an unknown edit kind");
				break;
			}
		}
		return issues;
	}

	ValidatedEngineCreateInfo::ValidatedEngineCreateInfo(EngineCreateInfo create_info)
		: create_info_(std::move(create_info))
	{
	}

	const EngineCreateInfo& ValidatedEngineCreateInfo::Value() const noexcept
	{
		return create_info_;
	}

	std::optional<ValidatedEngineCreateInfo> MakeValidatedEngineCreateInfo(
		EngineCreateInfo create_info,
		std::vector<std::string>* issues)
	{
		std::vector<std::string> validation_issues = ValidateEngineCreateInfo(create_info);
		if (issues != nullptr)
		{
			*issues = validation_issues;
		}
		if (!validation_issues.empty())
		{
			return std::nullopt;
		}
		return ValidatedEngineCreateInfo(std::move(create_info));
	}
}
