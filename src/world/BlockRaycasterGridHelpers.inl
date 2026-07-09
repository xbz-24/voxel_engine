namespace ve::gameplay
{
	namespace
	{
		/**
		 * Converts a world coordinate to its block cell.
		 */
		glm::ivec3 FloorToBlock(const glm::vec3& value)
		{
			return ve::world::grid::BlockPositionFromWorld(value);
		}

		/**
		 * Returns DDA step direction for one ray component.
		 */
		int StepFor(float direction)
		{
			if (direction > 0.0f) return 1;
			if (direction < 0.0f) return -1;
			return 0;
		}

		/**
		 * Computes distance to the next grid boundary for one axis.
		 */
		float InitialAxisDistance(float origin, float direction, int block, int step)
		{
			if (step == 0) return std::numeric_limits<float>::infinity();
			const float boundary = ve::world::grid::GridBoundaryForStep(block, step);
			return (boundary - origin) / direction;
		}

		/**
		 * Computes repeated DDA distance increment for one axis.
		 */
		float AxisDelta(float direction, int step)
		{
			return step == 0 ? std::numeric_limits<float>::infinity() : std::abs(1.0f / direction);
		}
