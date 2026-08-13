#include "EngineConfiguration.h"
#include "EngineConfigurationValidationDetail.h"

#include <utility>

namespace ve::engine
{
	std::vector<std::string> ValidateEngineCreateInfo(const EngineCreateInfo& create_info)
	{
		std::vector<std::string> issues;
		configuration_validation::ValidateWindowAndBackend(create_info, issues);
		configuration_validation::ValidateVoxelRenderStyle(create_info.voxel_render_style, issues);
		configuration_validation::ValidateWorldAndCamera(create_info, issues);
		return issues;
	}

	ValidatedEngineCreateInfo::ValidatedEngineCreateInfo(EngineCreateInfo create_info)
		: create_info_(std::move(create_info))
	{
	}

	const EngineCreateInfo& ValidatedEngineCreateInfo::Value() const noexcept
	{
		return create_info_;
	}

	std::optional<ValidatedEngineCreateInfo> MakeValidatedEngineCreateInfo(
		EngineCreateInfo create_info,
		std::vector<std::string>* issues)
	{
		std::vector<std::string> validation_issues = ValidateEngineCreateInfo(create_info);
		if (issues != nullptr) *issues = validation_issues;
		if (!validation_issues.empty()) return std::nullopt;
		return ValidatedEngineCreateInfo(std::move(create_info));
	}
}
