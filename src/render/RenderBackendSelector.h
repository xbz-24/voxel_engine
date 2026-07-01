#pragma once

#include "RenderApi.h"

namespace ve::rendering
{
	/** Chooses the runtime graphics API without leaking migration rules into Engine. */
	class RenderBackendSelector
	{
	public:
		/** @param configuration User or engine preference. @return Selected API plus headless mode flag. */
		[[nodiscard]] static RenderBackendSelection Select(const RenderBackendConfiguration& configuration) noexcept;

		/** @param configuration User or engine preference. @return Selected API. */
		[[nodiscard]] static GraphicsApi SelectApi(const RenderBackendConfiguration& configuration) noexcept;

		/** @param api Graphics API enum. @return Stable debug name. */
		[[nodiscard]] static const char* Name(GraphicsApi api) noexcept;
	};
}
