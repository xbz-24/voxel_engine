#include "RenderBackend.h"

#include "RenderBackendSelector.h"

namespace ve::rendering
{
	/** Returns the display name for the concrete backend API. */
	const char* RenderBackend::Name() const noexcept
	{
		return RenderBackendSelector::Name(Api());
	}
}
