#pragma once

#include "RenderApi.h"

namespace ve::rendering
{
	/** Chooses the runtime graphics API without leaking migration rules into Engine. */
	class RenderBackendSelector
	{
	public:
		/** @param configuration User or engine preference. @param requires_legacy_opengl True while old GL renderers are active. @return Selected API. */
		[[nodiscard]] static GraphicsApi SelectApi(const RenderBackendConfiguration& configuration, bool requires_legacy_opengl) noexcept;

		/** @param api Graphics API enum. @return Stable debug name. */
		[[nodiscard]] static const char* Name(GraphicsApi api) noexcept;
	};
}
