#include "FrameGraphBuilder.h"

namespace ve::rendering
{
	/// Creates a pass builder over a render graph.
	FrameGraphBuilder::FrameGraphBuilder(FrameGraph& graph) noexcept
		: graph_(graph)
	{
	}

	/// Adds one pass and returns the builder for chaining.
	FrameGraphBuilder& FrameGraphBuilder::AddPass(std::string name, std::function<void(FrameGraphContext&)> execute)
	{
		graph_.AddPass(ve::core::Move(name), ve::core::Move(execute));
		return *this;
	}

	FrameGraphBuilder& FrameGraphBuilder::AddPass(FrameGraphPass pass)
	{
		graph_.AddPass(ve::core::Move(pass));
		return *this;
	}
}
