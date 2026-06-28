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
	};

	class IEngineRuntimeFactory
	{
	public:
		virtual ~IEngineRuntimeFactory() = default;

		[[nodiscard]] virtual std::unique_ptr<IEngineRuntime> Create(ve::engine::EngineCreateInfo create_info) const = 0;
	};

	[[nodiscard]] const IEngineConfigValidator& DefaultEngineConfigValidator() noexcept;
	[[nodiscard]] const IEngineConfigTranslator& DefaultEngineConfigTranslator() noexcept;
	[[nodiscard]] const IEngineRuntimeFactory& DefaultEngineRuntimeFactory() noexcept;
}
