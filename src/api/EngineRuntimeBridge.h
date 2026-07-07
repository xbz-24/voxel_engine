#pragma once

#include "voxel/Engine.h"

#include "EngineConfiguration.h"

#include <memory>
#include <string>
#include <vector>

namespace voxel::detail
{
	class IEngineConfigValidator
	{
	public:
		virtual ~IEngineConfigValidator() = default;

		[[nodiscard]] virtual std::vector<std::string> Validate(const EngineConfig& config) const = 0;
	};

	class IEngineConfigTranslator
	{
	public:
		virtual ~IEngineConfigTranslator() = default;

		[[nodiscard]] virtual ve::engine::EngineCreateInfo Translate(const EngineConfig& config) const = 0;
	};

	class IEngineRuntime
	{
	public:
		virtual ~IEngineRuntime() = default;

		[[nodiscard]] virtual int Run() = 0;
		[[nodiscard]] virtual ve::engine::EngineStartupResult Start() = 0;
		[[nodiscard]] virtual bool Step() = 0;
		virtual void Shutdown() noexcept = 0;
		virtual void RequestStop() noexcept = 0;
		[[nodiscard]] virtual bool IsRunning() const noexcept = 0;
	};

	class IEngineRuntimeFactory
	{
	public:
		virtual ~IEngineRuntimeFactory() = default;

		[[nodiscard]] virtual std::unique_ptr<IEngineRuntime> Create(
			ve::engine::ValidatedEngineCreateInfo create_info) const = 0;
	};

	struct EngineServices
	{
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

	[[nodiscard]] const IEngineConfigValidator& DefaultEngineConfigValidator() noexcept;
	[[nodiscard]] const IEngineConfigTranslator& DefaultEngineConfigTranslator() noexcept;
	[[nodiscard]] const IEngineRuntimeFactory& DefaultEngineRuntimeFactory() noexcept;
	[[nodiscard]] EngineServices DefaultEngineServices() noexcept;
}
