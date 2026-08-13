#include "EngineInstanceImpl.h"

#include <optional>
#include <utility>

namespace voxel
{
	Engine::Impl::Impl(const EngineConfig& config)
		: Impl(config, detail::DefaultEngineServices())
	{
	}

	Engine::Impl::Impl(const EngineConfig& config, detail::EngineServices services)
		: validation_issues_(services.validator.Validate(config)),
		  on_log_(config.on_log)
	{
		if (!validation_issues_.empty())
		{
			return;
		}

		std::vector<std::string> create_info_issues;
		std::optional<ve::engine::ValidatedEngineCreateInfo> create_info =
			ve::engine::MakeValidatedEngineCreateInfo(
				services.translator.Translate(config),
				&create_info_issues);
		validation_issues_.insert(
			validation_issues_.end(),
			create_info_issues.begin(),
			create_info_issues.end());
		if (create_info.has_value())
		{
			runtime_ = services.runtime_factory.Create(std::move(*create_info));
		}
	}
}
