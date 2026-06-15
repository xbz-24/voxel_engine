#pragma once

#include "RenderBackend.h"

#include <memory>

namespace ve::rendering
{
	/** Creates concrete backend objects behind the common RenderBackend interface. */
	class RenderBackendFactory
	{
	public:
		/** @param api Graphics API to instantiate. @return Backend object, or null when unknown. */
		[[nodiscard]] static std::unique_ptr<RenderBackend> Create(GraphicsApi api);
	};
}
