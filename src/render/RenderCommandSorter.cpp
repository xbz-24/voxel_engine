#include "RenderCommandSorter.h"

#include <algorithm>

namespace ve::rendering
{
	namespace
	{
		/** @param left First sort key. @param right Second sort key. @return True when left submits before right. */
		bool ComesBefore(RenderSortKey left, RenderSortKey right) noexcept
		{
			if (left.layer != right.layer) return left.layer < right.layer;
			if (left.material != right.material) return left.material < right.material;
			return left.depth < right.depth;
		}
	}

	/** Sorts commands by stable render-state keys. */
	void RenderCommandSorter::SortForSubmission(std::span<RenderCommand> commands)
	{
		std::stable_sort(commands.begin(), commands.end(), [](const RenderCommand& left, const RenderCommand& right)
		{
			return ComesBefore(left.sort_key, right.sort_key);
		});
	}
}
