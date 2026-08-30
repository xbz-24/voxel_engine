#include "EngineWorldGeometryMath.h"

#include <limits>

namespace voxel::detail
{
	bool TryBlockCoordinate(WideBlockCoordinate value, int& destination) noexcept
	{
		if (value < std::numeric_limits<int>::lowest()
			|| value > std::numeric_limits<int>::max())
		{
			return false;
		}
		destination = static_cast<int>(value);
		return true;
	}

	bool TryCenteredBlockSpan(int center, int radius, int& first, int& second) noexcept
	{
		return TryBlockCoordinate(WidenBlockCoordinate(center) - WidenBlockCoordinate(radius), first)
			&& TryBlockCoordinate(WidenBlockCoordinate(center) + WidenBlockCoordinate(radius), second);
	}
}
