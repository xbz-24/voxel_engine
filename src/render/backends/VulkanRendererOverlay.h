#pragma once

#include <cstdint>

namespace ve::rendering
{
	/** Mutable visibility state for the backend-owned diagnostics overlay. */
	struct VulkanOverlaySettings
	{
		bool show_window = false;
		bool show_metrics = true;
	};

	/** Backend-neutral runtime measurements displayed by the Vulkan overlay. */
	struct VulkanRendererStats
	{
		int displayed_fps = 0;
		double delta_seconds = 0.0;
		double gpu_frame_ms = 0.0;
		double present_cpu_ms = 0.0;
		double mesh_rebuild_cpu_ms = 0.0;
		double mesh_upload_cpu_ms = 0.0;
		std::uint32_t index_count = 0;
		std::uint32_t shadow_index_count = 0;
		std::uint32_t rebuilt_chunk_count = 0;
		std::uint32_t cached_chunk_count = 0;
		std::uint64_t world_revision = 0;
		bool gpu_timing_valid = false;
		bool gpu_renderer_active = false;
	};
}
