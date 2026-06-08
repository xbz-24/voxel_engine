#include "FrameTimer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ve::time
{
	FrameTimer::FrameTimer()
		: _deltaSeconds(0.0),
		  _lastFrameTimestampSeconds(0.0),
		  _fpsAccumulatorSeconds(0.0),
		  _currentFrameCount(0),
		  _displayedFps(0)
	{
	}

	void FrameTimer::Tick()
	{
		const double currentFrameTimestampSeconds = static_cast<double>(glfwGetTime());
		_deltaSeconds = currentFrameTimestampSeconds - _lastFrameTimestampSeconds;
		_lastFrameTimestampSeconds = currentFrameTimestampSeconds;

		_currentFrameCount++;
		_fpsAccumulatorSeconds += _deltaSeconds;

		if (_fpsAccumulatorSeconds >= 1.0)
		{
			_displayedFps = _currentFrameCount;
			_currentFrameCount = 0;
			_fpsAccumulatorSeconds = 0.0;
		}
	}

	double FrameTimer::DeltaSeconds() const noexcept
	{
		return _deltaSeconds;
	}

	int FrameTimer::DisplayedFps() const noexcept
	{
		return _displayedFps;
	}
}

