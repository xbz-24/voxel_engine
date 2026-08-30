#pragma once

#include "RuntimeHostConfiguration.h"

namespace ve::engine
{
	struct EngineCreateInfo;

	[[nodiscard]] RuntimeHostConfiguration CreateRuntimeHostConfiguration(
		const EngineCreateInfo& create_info);
}
