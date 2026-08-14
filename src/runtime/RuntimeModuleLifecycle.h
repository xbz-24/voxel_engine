#pragma once

#include "RuntimeModule.h"

#include <memory>

namespace ve::engine
{
	/** Owns one runtime module and its initialize-to-shutdown obligation. */
	class RuntimeModuleLifecycle
	{
	public:
		explicit RuntimeModuleLifecycle(std::unique_ptr<IRuntimeModule> module) noexcept;
		~RuntimeModuleLifecycle() noexcept;

		[[nodiscard]] EngineStartupResult Initialize(RuntimeModuleContext& context);
		void RunFrame(RuntimeModuleContext& context);
		void Shutdown() noexcept;
		[[nodiscard]] bool HasModule() const noexcept;
		[[nodiscard]] bool IsActive() const noexcept;

	private:
		std::unique_ptr<IRuntimeModule> module_;
		bool shutdown_pending_ = false;
		bool initialized_ = false;
	};
}
