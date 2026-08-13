#include "EngineConfiguration.h"
#include "EngineConfigurationValidationDetail.h"

#include <cmath>

namespace ve::engine::configuration_validation
{
	namespace
	{
		[[nodiscard]] bool IsFinite(glm::vec3 value) noexcept
		{
			return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
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
	}

	void ValidateWorldAndCamera(
		const EngineCreateInfo& create_info,
		std::vector<std::string>& issues)
	{
		if (create_info.world_size_chunks <= 0)
			issues.push_back("world_size_chunks must be greater than zero");
		if (create_info.render_distance_chunks < 0)
			issues.push_back("render_distance_chunks must be zero or greater");
		if (create_info.has_custom_camera && !IsFinite(create_info.camera_position))
			issues.push_back("camera_position must contain finite values");
		if (create_info.has_custom_camera && !IsFinite(create_info.camera_look_at))
			issues.push_back("camera_look_at must contain finite values");

		for (const WorldBlockEdit& world_edit : create_info.world_edits)
		{
			if (IsKnownWorldBlockEditKind(world_edit.kind)) continue;
			issues.push_back("world_edits contains an unknown edit kind");
			break;
		}
	}
}
