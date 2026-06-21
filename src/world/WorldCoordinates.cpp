#include "WorldCoordinates.h"

namespace ve::world::coordinates
{
	int FloorDiv(int value, int divisor)
	{
		int result = value / divisor;
		const int remainder = value % divisor;
		if (remainder != 0 && ((remainder < 0) != (divisor < 0)))
		{
			result--;
		}
		return result;
	}

	int PositiveMod(int value, int divisor)
	{
		const int result = value % divisor;
		return result < 0 ? result + divisor : result;
	}
}
