#include "EngineRuntimeBridge.h"
#include "EngineConfigTranslatorInternal.h"

#include "CoreTypes.h"

#include <algorithm>
#include <utility>

namespace voxel::detail::config_translation
{
namespace
{
class EngineConfigTranslator final : public IEngineConfigTranslator
		{
		public:
			[[nodiscard]] ve::engine::EngineCreateInfo Translate(const EngineConfig& config) const override
			{
				ve::engine::EngineCreateInfo result{};
				ApplyWindowConfigurationToCreateInfo(result, config);
				ApplyStartupRenderingConfigurationToCreateInfo(result, config);
				ApplyCameraConfigurationToCreateInfo(result, config);
				ApplyRuntimeLayoutToCreateInfo(result, config);
				ApplyStaticModelSceneToCreateInfo(result, config);
				ApplyWorldConfigurationToCreateInfo(result, config);
				AppendAssetSearchRootsToCreateInfo(result, config.assets);
				result.logging = ToInternalLogging(config.logging);
				BindPublicUpdateCallback(result, config.on_update);
				BindPublicDiagnosticsCallback(result, config.on_diagnostics);
				result.on_log = config.on_log;
				return result;
			}
		};
	}

	const IEngineConfigTranslator& TranslatorInstance() noexcept
	{
		static const EngineConfigTranslator translator{};
		return translator;
	}
}
