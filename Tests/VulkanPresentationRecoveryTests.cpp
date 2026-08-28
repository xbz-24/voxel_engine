#include <doctest/doctest.h>

#include "VulkanFrameOrchestratorPresentation.h"
#include "VulkanFrameTypes.h"

#include <array>

namespace
{
	using ve::rendering::VulkanFrameResult;

	struct PresentationCase
	{
		VkResult result;
		VulkanFrameResult expected;
		bool acquire_can_continue;
	};

	constexpr std::array PresentationCases{
		PresentationCase{ VK_SUCCESS, VulkanFrameResult::Presented, true },
		PresentationCase{ VK_SUBOPTIMAL_KHR, VulkanFrameResult::RefreshRequired, true },
		PresentationCase{ VK_ERROR_OUT_OF_DATE_KHR, VulkanFrameResult::RefreshRequired, false },
		PresentationCase{ VK_ERROR_DEVICE_LOST, VulkanFrameResult::Failed, false }
	};
}

TEST_CASE("Vulkan presentation results classify recovery without a GPU")
{
	for (const PresentationCase& presentation_case : PresentationCases)
	{
		CAPTURE(presentation_case.result);
		CHECK(ve::rendering::ClassifyVulkanPresentationResult(presentation_case.result) ==
			presentation_case.expected);
		CHECK(ve::rendering::VulkanAcquireCanContinue(presentation_case.result) ==
			presentation_case.acquire_can_continue);
	}
}

TEST_CASE("Vulkan frame result combination keeps the most severe outcome")
{
	using enum VulkanFrameResult;
	struct CombinationCase
	{
		VulkanFrameResult first;
		VulkanFrameResult second;
		VulkanFrameResult expected;
	};
	constexpr std::array cases{
		CombinationCase{ Presented, Presented, Presented },
		CombinationCase{ Presented, RefreshRequired, RefreshRequired },
		CombinationCase{ RefreshRequired, Presented, RefreshRequired },
		CombinationCase{ RefreshRequired, RefreshRequired, RefreshRequired },
		CombinationCase{ Presented, Failed, Failed },
		CombinationCase{ Failed, Presented, Failed },
		CombinationCase{ RefreshRequired, Failed, Failed },
		CombinationCase{ Failed, RefreshRequired, Failed },
		CombinationCase{ Failed, Failed, Failed }
	};

	for (const CombinationCase& combination : cases)
	{
		CHECK(ve::rendering::CombineVulkanFrameResults(
			combination.first, combination.second) == combination.expected);
	}
}
