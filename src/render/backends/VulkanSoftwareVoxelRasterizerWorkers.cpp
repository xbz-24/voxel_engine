#include "VulkanSoftwareVoxelRasterizer.h"

#include "WorkerPolicy.h"

#include <algorithm>

namespace ve::rendering
{
	namespace
	{
		constexpr std::size_t kSamplesPerBatch = 512u;
	}
	void VulkanSoftwareVoxelRasterizer::EnsureWorkers()
	{
		if (!workers_.empty()) return;
		const std::size_t worker_count = static_cast<std::size_t>(ve::tasks::DefaultWorkerCount(1));
		std::uint64_t initial_generation = 0;
		{
			std::lock_guard lock(work_mutex_);
			initial_generation = work_generation_;
		}
		workers_.reserve(worker_count);
		for (std::size_t index = 0; index < worker_count; ++index)
		{
			workers_.emplace_back([this, initial_generation] { RenderWorkerLoop(initial_generation); });
		}
	}
	void VulkanSoftwareVoxelRasterizer::RenderSamplesMultithreaded(const RasterWork& work)
	{
		EnsureWorkers();
		if (workers_.empty() || ray_cache_.size() < 512u)
		{
			RenderSamples(work, 0, ray_cache_.size());
			return;
		}

		{
			std::lock_guard lock(work_mutex_);
			current_work_ = work;
			next_sample_index_.store(0);
			active_workers_ = workers_.size() + 1u;
			++work_generation_;
		}
		work_available_.notify_all();
		for (;;)
		{
			const std::size_t begin = next_sample_index_.fetch_add(kSamplesPerBatch);
			if (begin >= ray_cache_.size()) break;
			RenderSamples(work, begin, std::min(begin + kSamplesPerBatch, ray_cache_.size()));
		}
		CompleteWorkerBatch();

		std::unique_lock lock(work_mutex_);
		work_complete_.wait(lock, [this] { return active_workers_ == 0u; });
	}
	void VulkanSoftwareVoxelRasterizer::RenderWorkerLoop(std::uint64_t initial_generation)
	{
		std::uint64_t observed_generation = initial_generation;
		for (;;)
		{
			RasterWork work{};
			{
				std::unique_lock lock(work_mutex_);
				work_available_.wait(lock, [this, observed_generation] { return stop_workers_ || work_generation_ != observed_generation; });
				if (stop_workers_) return;
				observed_generation = work_generation_;
				work = current_work_;
			}

			for (;;)
			{
				const std::size_t begin = next_sample_index_.fetch_add(kSamplesPerBatch);
				if (begin >= ray_cache_.size()) break;
				RenderSamples(work, begin, std::min(begin + kSamplesPerBatch, ray_cache_.size()));
			}
			CompleteWorkerBatch();
		}
	}
	void VulkanSoftwareVoxelRasterizer::CompleteWorkerBatch()
	{
		std::lock_guard lock(work_mutex_);
		if (active_workers_ > 0u) --active_workers_;
		if (active_workers_ == 0u) work_complete_.notify_one();
	}
}
