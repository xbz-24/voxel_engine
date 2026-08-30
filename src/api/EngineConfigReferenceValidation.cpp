#include "EngineConfigValidationInternal.h"

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

namespace voxel::detail::config_validation
{
		void ValidateMaterialTextureBindingReferences(const Material& material,
			const std::set<std::string, std::less<>>& texture_names,
			std::vector<std::string>& issues)
		{
			const std::string material_label = MaterialLabel(material);
			ValidateOptionalReference(material.texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.normal_texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.roughness_texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.metallic_texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.occlusion_texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.emissive_texture, texture_names, material_label, "texture asset", issues);
		}

		void ValidateMaterialTextureReferences(
			const MaterialLibrary& materials,
			const AssetCatalog& assets,
			std::vector<std::string>& issues)
		{
			const std::set<std::string, std::less<>> texture_names = CollectNonEmptyNamesFrom(assets.textures);
			for (const Material& material : materials.materials)
			{
				ValidateMaterialTextureBindingReferences(material, texture_names, issues);
			}
		}

		void ValidateSceneGraphAssetReferences(
			const SceneGraph& scene_graph,
			const AssetCatalog& assets,
			const MaterialLibrary& materials,
			std::vector<std::string>& issues)
		{
			const std::set<std::string, std::less<>> model_names = CollectNonEmptyNamesFrom(assets.models);
			const std::set<std::string, std::less<>> material_names = CollectNonEmptyNamesFrom(materials.materials);
			for (const Entity& entity : scene_graph.entities)
			{
				const std::string entity_label = EntityLabel(entity);
				ValidateOptionalReference(entity.model, model_names, entity_label, "model asset", issues);
				ValidateOptionalReference(entity.material, material_names, entity_label, "material", issues);
			}
		}
}
