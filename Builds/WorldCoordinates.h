#pragma once

namespace ve::world::coordinates
{
	/**
	 * Divides two integers and rounds toward negative infinity.
	 *
	 * @param value Coordinate being divided.
	 * @param divisor Positive divisor.
	 * @return Floor-divided integer.
	 */
	int FloorDiv(int value, int divisor);

	/**
	 * Computes a positive modulo for local block coordinates.
	 *
	 * @param value Coordinate being reduced.
	 * @param divisor Positive divisor.
	 * @return Modulo result in the 0..divisor-1 range.
	 */
	int PositiveMod(int value, int divisor);
}
