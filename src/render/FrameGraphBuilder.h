#pragma once

#include "FrameGraph.h"

namespace ve::rendering
{
	/** Fluent helper used to register passes on a frame graph. */
	class FrameGraphBuilder
	{
	public:
		/** @param graph Graph receiving declared passes. */
		explicit FrameGraphBuilder(FrameGraph& graph) noexcept;

		/** @param name Debug name. @param execute Callback run by the graph. @return This builder. */
		FrameGraphBuilder& AddPass(std::string name, std::function<void(FrameGraphContext&)> execute);

		/** @param pass Fully declared pass. @return This builder. */
		FrameGraphBuilder& AddPass(FrameGraphPass pass);

	private:
		FrameGraph& graph_;
	};
}
