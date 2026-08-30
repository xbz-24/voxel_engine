#pragma once

#include "VulkanRendererOverlay.h"

#include <volk.h>

namespace ve::rendering::detail
{
	void CheckVkResult(VkResult result);
	void DrawStats(const VulkanRendererStats& stats);
}
