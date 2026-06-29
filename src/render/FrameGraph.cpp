#include "FrameGraph.h"

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] bool ContainsResource(const ve::core::DynamicArray<FrameGraphResourceHandle>& resources,
			FrameGraphResourceHandle handle) noexcept
		{
			for (const FrameGraphResourceHandle resource : resources)
			{
				if (resource == handle) return true;
			}
			return false;
		}

		[[nodiscard]] bool MustRunBefore(const FrameGraphPass& candidate_producer,
			const FrameGraphPass& candidate_consumer) noexcept
		{
			for (const FrameGraphResourceHandle written : candidate_producer.writes)
			{
				if (ContainsResource(candidate_consumer.reads, written)) return true;
			}
			return false;
		}

		[[nodiscard]] ve::core::DynamicArray<ve::core::Index> BuildExecutionOrder(
			const ve::core::DynamicArray<FrameGraphPass>& passes)
		{
			ve::core::DynamicArray<ve::core::Index> order;
			order.reserve(passes.size());
			ve::core::DynamicArray<bool> scheduled(passes.size(), false);

			while (order.size() < passes.size())
			{
				bool made_progress = false;
				for (ve::core::Index candidate = 0; candidate < passes.size(); ++candidate)
				{
					if (scheduled[candidate]) continue;

					bool has_unscheduled_dependency = false;
					for (ve::core::Index producer = 0; producer < passes.size(); ++producer)
					{
						if (producer == candidate || scheduled[producer]) continue;
						if (MustRunBefore(passes[producer], passes[candidate]))
						{
							has_unscheduled_dependency = true;
							break;
						}
					}

					if (has_unscheduled_dependency) continue;
					scheduled[candidate] = true;
					order.push_back(candidate);
					made_progress = true;
				}

				if (!made_progress)
				{
					for (ve::core::Index index = 0; index < passes.size(); ++index)
					{
						if (!scheduled[index])
						{
							scheduled[index] = true;
							order.push_back(index);
						}
					}
				}
			}

			return order;
		}
	}

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
		for (const ve::core::Index pass_index : BuildExecutionOrder(passes_))
		{
			FrameGraphPass& pass = passes_[pass_index];
			if (pass.execute) pass.execute(context);
		}
		context.frame_index++;
	}

	ve::core::DynamicArray<std::string> FrameGraph::Validate() const
	{
		ve::core::DynamicArray<std::string> issues;
		for (const FrameGraphPass& pass : passes_)
		{
			for (const FrameGraphResourceHandle read : pass.reads)
			{
				if (read.index >= resources_.size())
				{
					issues.push_back("FrameGraph pass '" + pass.name + "' reads an unknown resource");
					continue;
				}

				bool has_producer = false;
				for (const FrameGraphPass& producer : passes_)
				{
					if (ContainsResource(producer.writes, read))
					{
						has_producer = true;
						break;
					}
				}
				if (!has_producer)
				{
					issues.push_back("FrameGraph pass '" + pass.name + "' reads resource '" +
						resources_[read.index] + "' without a producer");
				}
			}

			for (const FrameGraphResourceHandle write : pass.writes)
			{
				if (write.index >= resources_.size())
				{
					issues.push_back("FrameGraph pass '" + pass.name + "' writes an unknown resource");
				}
			}
		}
		return issues;
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
