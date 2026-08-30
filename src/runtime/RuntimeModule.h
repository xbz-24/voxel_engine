#pragma once

#include "EngineStartupResult.h"
#include "RuntimeModuleContext.h"

namespace ve::engine
{
	/**
	 * Content boundary hosted by EngineRuntime without knowledge of game state.
	 * Once Initialize is entered, the host calls Shutdown exactly once even when
	 * initialization fails or throws; Shutdown must tolerate partial state.
	 */
	class IRuntimeModule
	{
	public:
		virtual ~IRuntimeModule() = default;
		[[nodiscard]] virtual EngineStartupResult Initialize(RuntimeModuleContext& context) = 0;
		virtual void RunFrame(RuntimeModuleContext& context) = 0;
		virtual void Shutdown() noexcept = 0;
	};
}
