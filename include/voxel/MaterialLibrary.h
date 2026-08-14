#pragma once

#include "voxel/Material.h"

#include <string>
#include <vector>

namespace voxel
{
	/** @addtogroup voxel_sdk_materials
	 * @{
	 */
	/** Collection of named materials referenced by authored scene data. */
	struct MaterialLibrary
	{
		std::vector<Material> materials;

		MaterialLibrary& Add(Material material);
		[[nodiscard]] std::vector<std::string> Validate() const;
	};
	/** @} */
}
