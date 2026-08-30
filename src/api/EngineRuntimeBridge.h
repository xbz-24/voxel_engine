#pragma once

#include "EngineConfigValidator.h"
#include "voxel/Engine.h"

#include "EngineConfiguration.h"

#include <memory>
#include <string>
#include <vector>

namespace voxel::detail
{
	/** Internal translator from public SDK config objects to engine runtime create-info. */
	class IEngineConfigTranslator
	{
	public:
		virtual ~IEngineConfigTranslator() = default;

		/** Converts a validated public config into private runtime create-info. */
		[[nodiscard]] virtual ve::engine::EngineCreateInfo Translate(const EngineConfig& config) const = 0;
	};

	/** Runtime abstraction used by voxel::Engine without exposing private engine classes. */
	class IEngineRuntime
	{
	public:
		virtual ~IEngineRuntime() = default;

		/** Runs the runtime-owned loop to completion. */
		[[nodiscard]] virtual int Run() = 0;

		/** Starts runtime systems for an externally driven frame loop. */
		[[nodiscard]] virtual ve::engine::EngineStartupResult Start() = 0;

		/** Advances one externally driven frame. */
		[[nodiscard]] virtual bool Step() = 0;

		/** Releases runtime systems. */
		virtual void Shutdown() noexcept = 0;

		/** Requests shutdown at the next safe frame boundary. */
		virtual void RequestStop() noexcept = 0;

		/** Returns whether the runtime is currently started. */
		[[nodiscard]] virtual bool IsRunning() const noexcept = 0;
	};

	/** Factory seam that keeps public API code independent from the concrete runtime class. */
	class IEngineRuntimeFactory
	{
	public:
		virtual ~IEngineRuntimeFactory() = default;

		/** Creates a runtime from validated private create-info. */
		[[nodiscard]] virtual std::unique_ptr<IEngineRuntime> Create(
			ve::engine::ValidatedEngineCreateInfo create_info) const = 0;
	};

	/** Bundle of services used by public Engine instances and test seams. */
	struct EngineServices
	{
		/** Stores references to long-lived validation, translation, and runtime factory services. */
		EngineServices(const IEngineConfigValidator& validator_service,
			const IEngineConfigTranslator& translator_service,
			const IEngineRuntimeFactory& runtime_factory_service) noexcept
			: validator(validator_service),
			  translator(translator_service),
			  runtime_factory(runtime_factory_service)
		{
		}

		const IEngineConfigValidator& validator;
		const IEngineConfigTranslator& translator;
		const IEngineRuntimeFactory& runtime_factory;
	};

	/** Returns the default public-to-private config translator singleton. */
	[[nodiscard]] const IEngineConfigTranslator& DefaultEngineConfigTranslator() noexcept;

	/** Returns the default concrete runtime factory singleton. */
	[[nodiscard]] const IEngineRuntimeFactory& DefaultEngineRuntimeFactory() noexcept;

	/** Returns the default service bundle used by voxel::Engine. */
	[[nodiscard]] EngineServices DefaultEngineServices() noexcept;
}
