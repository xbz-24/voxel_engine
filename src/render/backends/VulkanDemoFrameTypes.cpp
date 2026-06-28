#include "VulkanDemoFrameTypes.h"

namespace ve::rendering
{
	VulkanSoftwareVoxelRasterizerFrame::VulkanSoftwareVoxelRasterizerFrame(
		const ve::world::World& frame_world,
		const Camera& frame_camera,
		VkExtent2D frame_extent,
		VkFormat frame_format,
		int frame_displayed_fps,
		double frame_delta_seconds,
		VulkanFrameTiming frame_timing,
		const VulkanDemoInput& frame_input,
		VulkanDemoSettings& frame_settings) noexcept
		: world(frame_world),
		  camera(frame_camera),
		  extent(frame_extent),
		  format(frame_format),
		  displayed_fps(frame_displayed_fps),
		  delta_seconds(frame_delta_seconds),
		  previous_timing(frame_timing),
		  input(frame_input),
		  settings(frame_settings)
	{
	}
}
