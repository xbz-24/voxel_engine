#pragma once

#include "RenderApi.h"

namespace ve::rendering
{
	/** Common object-oriented contract implemented by every graphics backend. */
	class RenderBackend
	{
	public:
		/** Releases backend resources through the concrete implementation. */
		virtual ~RenderBackend() = default;

		/** @return Graphics API represented by this backend. */
		[[nodiscard]] virtual GraphicsApi Api() const noexcept = 0;

		/** @return Runtime feature flags and availability for this backend. */
		[[nodiscard]] virtual RenderBackendCapabilities Capabilities() const noexcept = 0;

		/** @return Human-readable backend name for logs and UI. */
		[[nodiscard]] const char* Name() const noexcept;
	};
}
