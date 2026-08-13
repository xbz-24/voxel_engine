#include "VulkanGpuChunkRendererVertexInput.h"

#include "VulkanGpuChunkRendererTypes.h"

#include <cstddef>

namespace ve::rendering::detail
{
	void VulkanGpuVertexInputDescriptions::Configure(VulkanGpuVertexLayout layout) noexcept
	{
		binding.binding = 0u;
		binding.stride = sizeof(VoxelVertex);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		attributes = {
			VkVertexInputAttributeDescription{
				0u, 0u, VK_FORMAT_R32G32B32_SFLOAT,
				VertexAttributeOffset(offsetof(VoxelVertex, position)) },
			VkVertexInputAttributeDescription{
				1u, 0u, VK_FORMAT_R8G8B8A8_UNORM,
				VertexAttributeOffset(offsetof(VoxelVertex, color_rgba8)) },
			VkVertexInputAttributeDescription{
				2u, 0u, VK_FORMAT_R32_SFLOAT,
				VertexAttributeOffset(offsetof(VoxelVertex, light)) },
			VkVertexInputAttributeDescription{
				3u, 0u, VK_FORMAT_R8G8B8A8_SNORM,
				VertexAttributeOffset(offsetof(VoxelVertex, normal_snorm8)) }
		};
		const bool uses_vertices = layout != VulkanGpuVertexLayout::None;
		const bool position_only = layout == VulkanGpuVertexLayout::VoxelPosition;
		create_info.vertexBindingDescriptionCount = uses_vertices ? 1u : 0u;
		create_info.pVertexBindingDescriptions = uses_vertices ? &binding : nullptr;
		create_info.vertexAttributeDescriptionCount = uses_vertices
			? (position_only ? 1u : RenderElementCount(attributes.size()))
			: 0u;
		create_info.pVertexAttributeDescriptions = uses_vertices ? attributes.data() : nullptr;
	}
}
