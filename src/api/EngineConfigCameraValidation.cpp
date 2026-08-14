#include "EngineConfigValidationInternal.h"

#include <cmath>
#include <limits>
#include <string_view>

namespace voxel::detail::config_validation
{
	namespace
	{
		[[nodiscard]] bool IsFinite(Vec3 value) noexcept
		{
			return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
		}

		[[nodiscard]] bool IsSupportedGridCoordinate(float value) noexcept
		{
			constexpr double minimum_exclusive =
				static_cast<double>(std::numeric_limits<int>::lowest());
			constexpr double maximum_exclusive =
				static_cast<double>(std::numeric_limits<int>::max()) + 1.0;
			const double promoted_value = static_cast<double>(value);
			return promoted_value > minimum_exclusive && promoted_value < maximum_exclusive;
		}

		[[nodiscard]] bool IsWithinSupportedGridRange(Vec3 value) noexcept
		{
			return IsSupportedGridCoordinate(value.x) &&
				IsSupportedGridCoordinate(value.y) && IsSupportedGridCoordinate(value.z);
		}

		void ValidateCameraPoint(Vec3 value,
			std::string_view label,
			std::vector<std::string>& issues)
		{
			if (!IsFinite(value))
				issues.push_back(std::string{ label } + " must contain finite values");
			else if (!IsWithinSupportedGridRange(value))
				issues.push_back(std::string{ label } +
					" must stay within the supported grid coordinate range");
		}
	}

	void ValidateCameraConfiguration(const EngineConfig& config,
		std::vector<std::string>& issues)
	{
		if (!config.camera.has_custom_view) return;
		ValidateCameraPoint(config.camera.position, "camera.position", issues);
		ValidateCameraPoint(config.camera.look_at, "camera.look_at", issues);
	}
}
