#include "EngineRuntimeBridge.h"

#include <algorithm>
#include <concepts>
#include <cmath>
#include <filesystem>
#include <ranges>
#include <set>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace voxel::detail
{
	const IEngineConfigValidator& DefaultEngineConfigValidator() noexcept
	{
		static const EngineConfigValidator validator{};
		return validator;
	}
}