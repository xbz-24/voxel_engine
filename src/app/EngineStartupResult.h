#pragma once

#include <string>
#include <utility>

namespace ve::engine
{
	enum class EngineStartupFailure
	{
		None,
		InvalidCreateInfo,
		WindowInitializationFailed,
		RenderBackendUnavailable,
		UnsupportedRenderBackend,
		RenderBackendInitializationFailed,
		RenderFrameRendererInitializationFailed,
		RenderViewCreationFailed
	};

	struct EngineStartupResult
	{
		EngineStartupFailure failure = EngineStartupFailure::None;
		std::string message;

		[[nodiscard]] static EngineStartupResult Success()
		{
			return {};
		}

		[[nodiscard]] static EngineStartupResult Failure(
			EngineStartupFailure failure,
			std::string message)
		{
			return EngineStartupResult{ failure, std::move(message) };
		}

		[[nodiscard]] explicit operator bool() const noexcept
		{
			return failure == EngineStartupFailure::None;
		}
	};
}
