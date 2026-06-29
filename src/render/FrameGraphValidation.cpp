#include "FrameGraph.h"

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] bool PassWritesResource(
			const FrameGraphPass& possible_producer_pass,
			FrameGraphResourceHandle resource_read_by_consumer) noexcept
		{
			for (const FrameGraphResourceHandle resource_written_by_producer : possible_producer_pass.writes)
			{
				if (resource_written_by_producer == resource_read_by_consumer) return true;
			}
			return false;
		}

		[[nodiscard]] bool AnyPassWritesResource(
			const ve::core::DynamicArray<FrameGraphPass>& passes,
			FrameGraphResourceHandle resource_read_by_consumer) noexcept
		{
			for (const FrameGraphPass& possible_producer_pass : passes)
			{
				if (PassWritesResource(possible_producer_pass, resource_read_by_consumer)) return true;
			}
			return false;
		}
	}

	ve::core::DynamicArray<std::string> FrameGraph::Validate() const
	{
		ve::core::DynamicArray<std::string> validation_issues;
		for (const FrameGraphPass& pass : passes_)
		{
			for (const FrameGraphResourceHandle resource_read_by_pass : pass.reads)
			{
				if (resource_read_by_pass.index >= resources_.size())
				{
					validation_issues.push_back("FrameGraph pass '" + pass.name + "' reads an unknown resource");
					continue;
				}

				if (!AnyPassWritesResource(passes_, resource_read_by_pass))
				{
					validation_issues.push_back("FrameGraph pass '" + pass.name + "' reads resource '" +
						resources_[resource_read_by_pass.index] + "' without a producer");
				}
			}

			for (const FrameGraphResourceHandle resource_written_by_pass : pass.writes)
			{
				if (resource_written_by_pass.index >= resources_.size())
				{
					validation_issues.push_back("FrameGraph pass '" + pass.name + "' writes an unknown resource");
				}
			}
		}
		return validation_issues;
	}
}
