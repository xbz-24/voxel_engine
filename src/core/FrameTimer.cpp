#include "FrameTimer.h"

namespace ve::time
{
	FrameTimer::FrameTimer()
		: _deltaSeconds(0.0),
		  _lastFrameTimestamp(std::chrono::steady_clock::now()),
		  _fpsAccumulatorSeconds(0.0),
		  _currentFrameCount(0),
		  _displayedFps(0)
	{
	}

	void FrameTimer::Tick()
	{
		const auto currentFrameTimestamp = std::chrono::steady_clock::now();
		_deltaSeconds = std::chrono::duration<double>(currentFrameTimestamp - _lastFrameTimestamp).count();
		_lastFrameTimestamp = currentFrameTimestamp;

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
