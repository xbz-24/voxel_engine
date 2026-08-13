#include "EngineRuntimeBridge.h"

#include "CoreTypes.h"

#include <algorithm>
#include <utility>

const IEngineConfigTranslator& DefaultEngineConfigTranslator() noexcept
	{
		static const EngineConfigTranslator translator{};
		return translator;
	}
}