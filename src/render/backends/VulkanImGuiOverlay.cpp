#include "VulkanImGuiOverlay.h"

#include "CoreTypes.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <algorithm>
#include <cstdint>
#include <string>

#include "VulkanImGuiOverlayPanels.inl"
#include "VulkanImGuiOverlayInitialize.inl"
#include "VulkanImGuiOverlayFrame.inl"
#include "VulkanImGuiOverlayLifecycle.inl"
