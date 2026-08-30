#pragma once

#include "ComputeDispatcher.h"

class RecordingComputeDispatcher final : public ve::rendering::ComputeDispatcher
{
public:
	explicit RecordingComputeDispatcher(bool supported) noexcept
		: supported_(supported)
	{
	}

	[[nodiscard]] bool IsSupported() const noexcept override
	{
		return supported_;
	}

	void Dispatch(
		ve::rendering::ComputeProgramHandle program,
		ve::rendering::ComputeDispatchSize size) const override
	{
		lastProgram = program;
		lastDispatchSize = size;
		dispatchCount++;
	}

	bool supported_ = false;
	mutable ve::rendering::ComputeProgramHandle lastProgram{};
	mutable ve::rendering::ComputeDispatchSize lastDispatchSize{};
	mutable int dispatchCount = 0;
};
