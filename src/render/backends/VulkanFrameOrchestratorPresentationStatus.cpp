#include "VulkanFrameOrchestratorPresentation.h"

namespace ve::rendering
{
	VulkanFrameResult ClassifyVulkanPresentationResult(VkResult result) noexcept
	{
		if (result == VK_SUCCESS) return VulkanFrameResult::Presented;
		if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			return VulkanFrameResult::RefreshRequired;
		}
		return VulkanFrameResult::Failed;
	}

	bool VulkanAcquireCanContinue(VkResult result) noexcept
	{
		return result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR;
	}

	VulkanFrameResult CombineVulkanFrameResults(
		VulkanFrameResult first,
		VulkanFrameResult second) noexcept
	{
		if (first == VulkanFrameResult::Failed || second == VulkanFrameResult::Failed)
		{
			return VulkanFrameResult::Failed;
		}
		if (first == VulkanFrameResult::RefreshRequired ||
			second == VulkanFrameResult::RefreshRequired)
		{
			return VulkanFrameResult::RefreshRequired;
		}
		return VulkanFrameResult::Presented;
	}
}
