#include "EngineRuntimeBridge.h"
#include "EngineConfigStaticSceneSupport.h"
#include "EngineConfigTranslatorInternal.h"

namespace voxel::detail::config_translation
{
	void ApplyStaticModelSceneToCreateInfo(ve::engine::EngineCreateInfo& create_info,
		const EngineConfig& config)
	{
		create_info.static_model_scene.reset();
		const static_scene::StaticModelSceneAnalysis analysis =
			static_scene::AnalyzeStaticModelScene(config);
		if (!analysis.runtime_candidate) return;
		create_info.static_model_scene = ve::engine::StaticModelSceneConfiguration{
			analysis.runtime_candidate->model_path,
			analysis.runtime_candidate->visible
		};
	}
}
