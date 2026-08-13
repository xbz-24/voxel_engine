#include "EngineConfigValidationInternal.h"

#include <string>

namespace voxel::detail::config_validation
{
	std::string MaterialLabel(const Material& material)
	{
		return material.name.empty() ? "material" : "material '" + material.name + "'";
	}

	std::string EntityLabel(const Entity& entity)
	{
		return entity.name.empty() ? "scene entity" : "scene entity '" + entity.name + "'";
	}
}
