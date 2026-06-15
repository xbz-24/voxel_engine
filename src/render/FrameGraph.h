#pragma once

#include "CoreTypes.h"

#include <functional>
#include <string>
#include <string_view>

namespace ve::rendering
{
	/** Per-frame data passed into render graph pass callbacks. */
	struct FrameGraphContext
	{
		ve::core::Index frame_index = 0;
	};

	/** Executable render graph node registered by high-level render code. */
	struct FrameGraphPass
	{
		std::string name;
		std::function<void(FrameGraphContext&)> execute;
	};

	/** Owns and executes render passes declared for a frame. */
	class FrameGraph
	{
	public:
		/** @param name Debug name for the pass. @param execute Callback run during Execute. */
		void AddPass(std::string name, std::function<void(FrameGraphContext&)> execute);

		/** @param context Per-frame render context. */
		void Execute(FrameGraphContext& context);

		/** Removes every registered pass. */
		void Clear();

		/** @return Number of passes scheduled this frame. */
		[[nodiscard]] ve::core::Index PassCount() const noexcept;

	private:
		ve::core::DynamicArray<FrameGraphPass> passes_;
	};
}
