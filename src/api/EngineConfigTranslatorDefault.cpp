#include "EngineRuntimeBridge.h"
#include "EngineConfigTranslatorInternal.h"

#include "CoreTypes.h"

#include <algorithm>
#include <utility>

namespace voxel::detail
{
const IEngineConfigTranslator& DefaultEngineConfigTranslator() noexcept
	{
		return config_translation::TranslatorInstance();
	}
}
