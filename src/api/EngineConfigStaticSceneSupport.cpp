#include "EngineConfigStaticSceneSupport.h"
#include "EngineConfigStaticScenePredicates.h"

#include <system_error>
#include <utility>

namespace voxel::detail::static_scene
{
	namespace
	{
		void AddIssueIf(bool condition, std::string issue, std::vector<std::string>& issues)
		{
			if (condition) issues.push_back(std::move(issue));
		}
	}

	StaticModelSceneAnalysis AnalyzeStaticModelScene(const EngineConfig& config)
	{
		StaticModelSceneAnalysis result{};
		result.has_asset_content = !config.assets.textures.empty() ||
			!config.assets.models.empty() || !config.assets.sounds.empty();
		result.has_scene_content = !config.scene_graph.entities.empty() ||
			!config.scene_graph.lights.empty() ||
			!support_detail::IsDefaultEnvironment(config.scene_graph.environment);
		if (config.assets.models.empty() || config.scene_graph.entities.empty()) return result;

		AddIssueIf(config.graphics_api != GraphicsApi::OpenGLCompatibility,
			"Static model scenes require OpenGLCompatibility", result.issues);
		AddIssueIf(config.assets.models.size() != 1,
			"Static model scenes require exactly one model asset", result.issues);
		AddIssueIf(!config.assets.search_roots.empty(),
			"Static model scenes do not support asset search roots", result.issues);
		AddIssueIf(!config.assets.textures.empty(),
			"Static model scenes do not support texture assets", result.issues);
		AddIssueIf(!config.assets.sounds.empty(),
			"Static model scenes do not support sound assets", result.issues);
		AddIssueIf(!config.materials.materials.empty(),
			"Static model scenes do not support MaterialLibrary", result.issues);
		AddIssueIf(config.scene_graph.composition_mode != SceneGraphCompositionMode::VoxelWorldAnchored,
			"Static model scenes require VoxelWorldAnchored composition", result.issues);
		AddIssueIf(config.scene_graph.entities.size() != 1,
			"Static model scenes require exactly one entity", result.issues);
		AddIssueIf(!config.scene_graph.lights.empty(),
			"Static model scenes do not support lights", result.issues);
		AddIssueIf(!support_detail::IsDefaultEnvironment(config.scene_graph.environment),
			"Static model scenes require the default environment", result.issues);
		if (config.assets.models.size() != 1 || config.scene_graph.entities.size() != 1) return result;

		const ModelAsset& model = config.assets.models.front();
		const Entity& entity = config.scene_graph.entities.front();
		const std::filesystem::path path = support_detail::EffectivePath(model);
		AddIssueIf(model.source.storage != AssetStorage::FilePath,
			"Static model scenes require a FilePath model asset", result.issues);
		AddIssueIf(model.source.hot_reload,
			"Static model scenes do not support model hot reload", result.issues);
		AddIssueIf(!model.source.archive_path.empty() || !model.source.embedded_data.empty(),
			"Static model scene FilePath sources must not contain archive or embedded payloads", result.issues);
		AddIssueIf(!model.path.empty() && !model.source.location.empty() && model.path != model.source.location,
			"Static model scene model path and source location must match", result.issues);
		AddIssueIf(path.empty() || !path.is_absolute(),
			"Static model scene model path must be absolute", result.issues);
		AddIssueIf(!support_detail::HasObjExtension(path),
			"Static model scene model path must have a .obj extension", result.issues);
		AddIssueIf(model.name.empty() || entity.model != model.name,
			"Static model scene entity must reference the registered model", result.issues);
		AddIssueIf(!entity.id.IsValid(), "Static model scene entity must have a valid id", result.issues);
		AddIssueIf(entity.parent.IsValid(), "Static model scene entity must be a root", result.issues);
		AddIssueIf(!entity.material.empty(), "Static model scene entity material must be empty", result.issues);
		AddIssueIf(!support_detail::IsFinite(entity.transform.position),
			"Static model scene entity position must be finite", result.issues);
		AddIssueIf(!support_detail::IsZero(entity.transform.rotation),
			"Static model scenes do not support entity rotation", result.issues);
		AddIssueIf(!support_detail::IsUnitScale(entity.transform.scale),
			"Static model scenes do not support entity scale", result.issues);
		if (result.issues.empty())
			result.runtime_candidate = StaticModelSceneSelection{
				path, entity.visible, entity.transform.position };
		std::error_code path_error;
		AddIssueIf(!std::filesystem::is_regular_file(path, path_error),
			"Static model scene model path must name an existing regular file", result.issues);
		if (result.issues.empty()) result.supported_scene = StaticModelSceneSelection{
			path, entity.visible, entity.transform.position };
		return result;
	}
}
