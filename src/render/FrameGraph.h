#pragma once

#include "CoreTypes.h"

#include <functional>
#include <string>

namespace ve::rendering
{
	/** Per-frame data passed into render graph pass callbacks. */
	struct FrameGraphContext
	{
		// TODO: Carry backend command encoder, frame resources, and render stats instead of only a frame index.
		ve::core::Index frame_index = 0;
	};

	/** Stable per-graph identifier for a declared render resource. */
	struct FrameGraphResourceHandle
	{
		ve::core::Index index = 0;

		[[nodiscard]] friend bool operator==(FrameGraphResourceHandle, FrameGraphResourceHandle) noexcept = default;
	};

	/** Executable render graph node registered by high-level render code. */
	struct FrameGraphPass
	{
		std::string name;
		ve::core::DynamicArray<FrameGraphResourceHandle> reads;
		ve::core::DynamicArray<FrameGraphResourceHandle> writes;
		std::function<void(FrameGraphContext&)> execute;
	};

	/** Owns and executes render passes declared for a frame. */
	class FrameGraph
	{
	public:
		// TODO: Topologically sort passes by declared reads/writes and validate missing producers.
		/** @param name Debug name for a transient frame resource. @return Handle used by passes. */
		[[nodiscard]] FrameGraphResourceHandle DeclareResource(std::string name);

		/** @param name Debug name for the pass. @param execute Callback run during Execute. */
		void AddPass(std::string name, std::function<void(FrameGraphContext&)> execute);

		/** @param pass Fully declared pass with resource dependencies and execute callback. */
		void AddPass(FrameGraphPass pass);

		/** @param context Per-frame render context. */
		void Execute(FrameGraphContext& context);

		/** Removes every registered pass. */
		void Clear();

		/** @return Number of passes scheduled this frame. */
		[[nodiscard]] ve::core::Index PassCount() const noexcept;

		/** @return Number of resources declared on the graph. */
		[[nodiscard]] ve::core::Index ResourceCount() const noexcept;

		/** @return Read-only pass declarations in execution order. */
		[[nodiscard]] const ve::core::DynamicArray<FrameGraphPass>& Passes() const noexcept;

	private:
		// TODO: Promote string resource names to descriptors with format, size, lifetime, and import/export flags.
		ve::core::DynamicArray<std::string> resources_;
		ve::core::DynamicArray<FrameGraphPass> passes_;
	};
}
