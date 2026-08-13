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

namespace voxel
{
	namespace
	{
		void ValidateMaterialNameIsPresentAndUnique(const Material& material,
			std::set<std::string>& material_names_seen,
			std::vector<std::string>& issues)
		{
			if (material.name.empty())
			{
				issues.push_back("material name must not be empty");
			}
			else if (!material_names_seen.insert(material.name).second)
			{
				issues.push_back("material name is duplicated: " + material.name);
			}
		}

		void ValidateMaterialScalarProperties(const Material& material,
			const std::string& material_label,
			std::vector<std::string>& issues)
		{
			ValidateColor(material.base_color, material_label + ".base_color", issues);
			ValidateFiniteRange(material.metallic, material_label + ".metallic", NormalizedFloatRange, issues);
			ValidateFiniteRange(material.roughness, material_label + ".roughness", NormalizedFloatRange, issues);
			ValidateNonNegative(material.emission, material_label + ".emission", issues);
		}

		void ValidateMaterialDefinition(const Material& material,
			std::set<std::string>& material_names_seen,
			std::vector<std::string>& issues)
		{
			ValidateMaterialNameIsPresentAndUnique(material, material_names_seen, issues);

			const std::string material_label = MaterialLabel(material);
			ValidateMaterialScalarProperties(material, material_label, issues);
		}

		void ValidateMaterials(const std::vector<Material>& materials, std::vector<std::string>& issues)
		{
			std::set<std::string> material_names_seen;
			for (const Material& material : materials)
			{
				ValidateMaterialDefinition(material, material_names_seen, issues);
			}
		}
	}
}
