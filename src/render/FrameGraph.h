#pragma once

#include "CoreTypes.h"
#include "FrameGraphResource.h"

#include <functional>
#include <string>

namespace ve::rendering
{
	struct FrameGraphExecutionStats
	{
		ve::core::Index executed_pass_count = 0;
	};

	/** Per-frame data passed into render graph pass callbacks. */
	struct FrameGraphContext
	{
		ve::core::Index frame_index = 0;
		void* backend_command_encoder = nullptr;
		void* frame_resources = nullptr;
		FrameGraphExecutionStats* stats = nullptr;
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
		/** @param name Debug name for a transient frame resource. @return Handle used by passes. */
		[[nodiscard]] FrameGraphResourceHandle DeclareResource(std::string name);

		/** @param descriptor Full resource metadata. @return Handle used by passes. */
		[[nodiscard]] FrameGraphResourceHandle DeclareResource(FrameGraphResourceDescriptor descriptor);

		/** @param name Debug name for the pass. @param execute Callback run during Execute. */
		void AddPass(std::string name, std::function<void(FrameGraphContext&)> execute);

		/** @param pass Fully declared pass with resource dependencies and execute callback. */
		void AddPass(FrameGraphPass pass);

		/** @param context Per-frame render context. */
		void Execute(FrameGraphContext& context);

		/** @return Validation issues for declared passes and resources. */
		[[nodiscard]] ve::core::DynamicArray<std::string> Validate() const;

		/** Removes every registered pass. */
		void Clear();

		/** @return Number of passes scheduled this frame. */
		[[nodiscard]] ve::core::Index PassCount() const noexcept;

		/** @return Number of resources declared on the graph. */
		[[nodiscard]] ve::core::Index ResourceCount() const noexcept;

		/** @return Read-only pass declarations in execution order. */
		[[nodiscard]] const ve::core::DynamicArray<FrameGraphPass>& Passes() const noexcept;

		/** @return Read-only resource descriptors. */
		[[nodiscard]] const ve::core::DynamicArray<FrameGraphResourceDescriptor>& Resources() const noexcept;

	private:
		ve::core::DynamicArray<FrameGraphResourceDescriptor> resources_;
		ve::core::DynamicArray<FrameGraphPass> passes_;
	};
}
