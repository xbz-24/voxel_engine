#pragma once

#include "EngineConfigValidator.h"

#include <set>
#include <span>
#include <string_view>

namespace voxel::detail::config_validation
{
	[[nodiscard]] std::string MaterialLabel(const Material& material);
	[[nodiscard]] std::string EntityLabel(const Entity& entity);
	void ValidateFiniteRange(
		float value,
		std::string label,
		FloatRange range,
		std::vector<std::string>& issues);
	void ValidateNonNegative(
		float value,
		std::string label,
		std::vector<std::string>& issues);
	void ValidateColor(
		Color color,
		const std::string& label,
		std::vector<std::string>& issues);
	[[nodiscard]] std::set<std::string, std::less<>> CollectNonEmptyNamesFrom(
		const std::vector<TextureAsset>& named_values);
	[[nodiscard]] std::set<std::string, std::less<>> CollectNonEmptyNamesFrom(
		const std::vector<ModelAsset>& named_values);
	[[nodiscard]] std::set<std::string, std::less<>> CollectNonEmptyNamesFrom(
		const std::vector<Material>& named_values);
	void ValidateOptionalReference(
		std::string_view reference,
		const std::set<std::string, std::less<>>& known_names,
		const std::string& owner_label,
		std::string_view reference_kind,
		std::vector<std::string>& issues);

	void ValidateAssets(
		const std::vector<TextureAsset>& assets,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues);
	void ValidateAssets(
		const std::vector<ModelAsset>& assets,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues);
	void ValidateAssets(
		const std::vector<SoundAsset>& assets,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues);
	void ValidateSearchRoots(
		std::span<const std::string> search_roots,
		bool require_existing_directories,
		std::vector<std::string>& issues);
	void ValidateMaterials(
		const std::vector<Material>& materials,
		std::vector<std::string>& issues);
	void ValidateMaterialTextureReferences(
		const MaterialLibrary& materials,
		const AssetCatalog& assets,
		std::vector<std::string>& issues);
	void ValidateSceneGraphAssetReferences(
		const SceneGraph& scene_graph,
		const AssetCatalog& assets,
		const MaterialLibrary& materials,
		std::vector<std::string>& issues);
	void ValidateWindowAndWorldConfiguration(
		const EngineConfig& config,
		std::vector<std::string>& issues);
	void ValidateGraphicsBackendSupport(
		const EngineConfig& config,
		std::vector<std::string>& issues);
	void ValidateVoxelRenderLightingAndFog(
		const VoxelRenderStyle& style,
		std::vector<std::string>& issues);
	void ValidateVoxelRenderEffects(
		const VoxelRenderStyle& style,
		std::vector<std::string>& issues);

	[[nodiscard]] const IEngineConfigValidator& ValidatorInstance() noexcept;
}
