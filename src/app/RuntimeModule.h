#pragma once

#include "EngineStartupResult.h"
#include "RuntimeModuleContext.h"

namespace ve::engine
{
	/** Content boundary hosted by EngineRuntime without knowledge of game state. */
	class IRuntimeModule
	{
	public:
		virtual ~IRuntimeModule() = default;
		[[nodiscard]] virtual EngineStartupResult Initialize(RuntimeModuleContext& context) = 0;
		virtual void RunFrame(RuntimeModuleContext& context) = 0;
		virtual void Shutdown() noexcept = 0;
	};
}
