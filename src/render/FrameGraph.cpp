#include "FrameGraph.h"

namespace ve::rendering
{
	FrameGraphResourceHandle FrameGraph::DeclareResource(std::string name)
	{
		const FrameGraphResourceHandle handle{ resources_.size() };
		resources_.push_back(ve::core::Move(name));
		return handle;
	}

	/// Adds one executable render pass.
	void FrameGraph::AddPass(std::string name, std::function<void(FrameGraphContext&)> execute)
	{
		FrameGraphPass pass{};
		pass.name = ve::core::Move(name);
		pass.execute = ve::core::Move(execute);
		AddPass(ve::core::Move(pass));
	}

	void FrameGraph::AddPass(FrameGraphPass pass)
	{
		passes_.push_back(ve::core::Move(pass));
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
		resources_.clear();
	}

	/// Returns number of passes scheduled this frame.
	ve::core::Index FrameGraph::PassCount() const noexcept { return passes_.size(); }

	ve::core::Index FrameGraph::ResourceCount() const noexcept { return resources_.size(); }

	const ve::core::DynamicArray<FrameGraphPass>& FrameGraph::Passes() const noexcept { return passes_; }
}
