#include "VoxelSandboxRuntimeConfiguration.h"

#include "EngineConfiguration.h"

namespace ve::engine
{
	RuntimeHostConfiguration CreateRuntimeHostConfiguration(const EngineCreateInfo& create_info)
	{
		return RuntimeHostConfiguration{
			create_info.window,
			create_info.render_backend,
			create_info.logging,
			create_info.vsync,
			create_info.asset_search_roots,
			create_info.on_log
		};
	}
}
