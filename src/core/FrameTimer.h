#pragma once

#include <chrono>

namespace ve::time
{
	/**
	 * Tracks frame delta time and a once-per-second FPS value.
	 */
	class FrameTimer
	{
	public:
		/**
		 * Creates a timer with zeroed counters.
		 */
		FrameTimer();

		/**
		 * Updates delta time and FPS counters for the current frame.
		 */
		void Tick();

		/**
		 * Returns seconds elapsed since the previous frame.
		 *
		 * @return Frame delta in seconds.
		 */
		double DeltaSeconds() const noexcept;

		/**
		 * Returns the last complete one-second FPS sample.
		 *
		 * @return Displayable frames-per-second value.
		 */
		int DisplayedFps() const noexcept;

	private:
		double _deltaSeconds;
		std::chrono::steady_clock::time_point _lastFrameTimestamp;
		double _fpsAccumulatorSeconds;
		int _currentFrameCount;
		int _displayedFps;
	};
}
