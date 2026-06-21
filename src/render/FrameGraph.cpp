#include "FrameGraph.h"

namespace ve::rendering
{
	/// Adds one executable render pass.
	void FrameGraph::AddPass(std::string name, std::function<void(FrameGraphContext&)> execute)
	{
		passes_.push_back(FrameGraphPass{ ve::core::Move(name), ve::core::Move(execute) });
	}

	/// Executes passes in dependency order.
	void FrameGraph::Execute(FrameGraphContext& context)
	{
		for (FrameGraphPass& pass : passes_)
		{
			if (pass.execute) pass.execute(context);
		}
		context.frame_index++;
	}

	/// Removes every registered pass.
	void FrameGraph::Clear()
	{
		passes_.clear();
	}

	/// Returns number of passes scheduled this frame.
	ve::core::Index FrameGraph::PassCount() const noexcept { return passes_.size(); }
}
