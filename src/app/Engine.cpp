#include "Engine.h"

#include <cassert>
#include <utility>

namespace
{
	void ApplyRuntimeSettings(
		ve::gameplay::RuntimeSettings& runtime_settings,
		const ve::engine::EngineCreateInfo& create_info) noexcept
	{
		runtime_settings.renderer.is_vsync_enabled = create_info.vsync;
		runtime_settings.renderer.show_debug_overlay = create_info.show_debug_overlay;
		runtime_settings.renderer.render_distance_chunks =
			ve::gameplay::ClampRenderDistanceChunks(create_info.render_distance_chunks);
		runtime_settings.renderer.backend_configuration = create_info.render_backend;
	}
}

EngineApplication::EngineApplication()
	: EngineApplication(ve::engine::EngineCreateInfo{})
{
}

EngineApplication::EngineApplication(ve::engine::EngineCreateInfo create_info)
{
	create_info_ = ve::engine::MakeValidatedEngineCreateInfo(
		std::move(create_info),
		&create_info_validation_issues_);
	if (create_info_ != std::nullopt)
	{
		ApplyRuntimeSettings(_runtimeSettings, create_info_->Value());
	}
}

EngineApplication::EngineApplication(ve::engine::ValidatedEngineCreateInfo create_info)
	: create_info_(std::move(create_info))
{
	ApplyRuntimeSettings(_runtimeSettings, create_info_->Value());
}

EngineApplication::~EngineApplication() = default;

const ve::engine::EngineCreateInfo& EngineApplication::CreateInfo() const noexcept
{
	assert(create_info_.has_value());
	return create_info_->Value();
}

bool EngineApplication::HasValidCreateInfo() const noexcept
{
	return create_info_.has_value();
}
