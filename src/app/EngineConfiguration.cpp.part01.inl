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
