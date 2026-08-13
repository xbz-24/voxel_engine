#include "Engine.h"

#include "EngineRuntime.h"

#include <cassert>
#include <utility>

EngineApplication::EngineApplication()
	: EngineApplication(ve::engine::EngineCreateInfo{})
{
}

EngineApplication::EngineApplication(ve::engine::EngineCreateInfo create_info)
{
	create_info_ = ve::engine::MakeValidatedEngineCreateInfo(
		std::move(create_info),
		&create_info_validation_issues_);
}

EngineApplication::EngineApplication(ve::engine::ValidatedEngineCreateInfo create_info)
{
	create_info_ = std::move(create_info);
}

EngineApplication::~EngineApplication()
{
	Shutdown();
}

const ve::engine::EngineCreateInfo& EngineApplication::CreateInfo() const noexcept
{
	assert(create_info_.has_value());
	return create_info_->Value();
}

bool EngineApplication::HasValidCreateInfo() const noexcept
{
	return create_info_.has_value();
}
