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

		[[nodiscard]] const FrameGraphResourceDescriptor& Resource(
			const ve::core::DynamicArray<FrameGraphResourceDescriptor>& resources,
			FrameGraphResourceHandle handle) noexcept
		{
			return resources[handle.index];
		}

		[[nodiscard]] bool ResourceIsImported(const FrameGraphResourceDescriptor& descriptor) noexcept
		{
			return descriptor.imported || descriptor.lifetime == FrameGraphResourceLifetime::Imported;
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

				const FrameGraphResourceDescriptor& resource = Resource(resources_, resource_read_by_pass);
				if (!ResourceIsImported(resource) && !AnyPassWritesResource(passes_, resource_read_by_pass))
				{
					validation_issues.push_back("FrameGraph pass '" + pass.name + "' reads resource '" +
						resource.name + "' without a producer");
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
