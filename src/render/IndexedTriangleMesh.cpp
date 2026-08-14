#include "IndexedTriangleMesh.h"

#include <algorithm>

namespace ve::rendering
{
	bool IndexedTriangleMeshDescription::IsValid() const noexcept
	{
		if (vertices.empty() || indices.empty() || indices.size() % 3U != 0U) return false;
		return std::ranges::all_of(indices, [vertex_count = vertices.size()](std::uint32_t index)
		{
			return index < vertex_count;
		});
	}
}
