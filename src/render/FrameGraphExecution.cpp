#include "FrameGraph.h"

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] bool ResourceListContains(
			const ve::core::DynamicArray<FrameGraphResourceHandle>& resource_handles,
			FrameGraphResourceHandle resource_to_find) noexcept
		{
			for (const FrameGraphResourceHandle listed_resource : resource_handles)
			{
				if (listed_resource == resource_to_find) return true;
			}
			return false;
		}

		[[nodiscard]] bool ProducerWritesResourceReadByConsumer(
			const FrameGraphPass& possible_producer_pass,
			const FrameGraphPass& possible_consumer_pass) noexcept
		{
			for (const FrameGraphResourceHandle resource_written_by_producer : possible_producer_pass.writes)
			{
				if (ResourceListContains(possible_consumer_pass.reads, resource_written_by_producer)) return true;
			}
			return false;
		}

		[[nodiscard]] bool PassStillWaitsForProducer(
			const ve::core::DynamicArray<FrameGraphPass>& passes,
			const ve::core::DynamicArray<bool>& pass_already_scheduled,
			ve::core::Index candidate_pass_index) noexcept
		{
			for (ve::core::Index possible_producer_index = 0; possible_producer_index < passes.size(); ++possible_producer_index)
			{
				if (possible_producer_index == candidate_pass_index || pass_already_scheduled[possible_producer_index]) continue;
				if (ProducerWritesResourceReadByConsumer(passes[possible_producer_index], passes[candidate_pass_index])) return true;
			}
			return false;
		}

		void AppendUnscheduledPasses(
			const ve::core::DynamicArray<bool>& pass_already_scheduled,
			ve::core::DynamicArray<ve::core::Index>& ordered_pass_indices)
		{
			for (ve::core::Index pass_index = 0; pass_index < pass_already_scheduled.size(); ++pass_index)
			{
				if (!pass_already_scheduled[pass_index]) ordered_pass_indices.push_back(pass_index);
			}
		}

		[[nodiscard]] ve::core::DynamicArray<ve::core::Index> BuildExecutionOrder(
			const ve::core::DynamicArray<FrameGraphPass>& passes)
		{
			ve::core::DynamicArray<ve::core::Index> ordered_pass_indices;
			ordered_pass_indices.reserve(passes.size());
			ve::core::DynamicArray<bool> pass_already_scheduled(passes.size(), false);

			while (ordered_pass_indices.size() < passes.size())
			{
				bool scheduled_at_least_one_pass = false;
				for (ve::core::Index candidate_pass_index = 0; candidate_pass_index < passes.size(); ++candidate_pass_index)
				{
					if (pass_already_scheduled[candidate_pass_index]) continue;
					if (PassStillWaitsForProducer(passes, pass_already_scheduled, candidate_pass_index)) continue;

					pass_already_scheduled[candidate_pass_index] = true;
					ordered_pass_indices.push_back(candidate_pass_index);
					scheduled_at_least_one_pass = true;
				}

				if (!scheduled_at_least_one_pass)
				{
					AppendUnscheduledPasses(pass_already_scheduled, ordered_pass_indices);
				}
			}

			return ordered_pass_indices;
		}
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
}
