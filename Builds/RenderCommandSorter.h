#pragma once

#include "RenderCommand.h"

#include <span>

namespace ve::rendering
{
	/** Sorts draw commands into a stable GPU-friendly submission order. */
	class RenderCommandSorter
	{
	public:
		/** @param commands Commands to sort by layer, material, then depth. */
		static void SortForSubmission(std::span<RenderCommand> commands);
	};
}
