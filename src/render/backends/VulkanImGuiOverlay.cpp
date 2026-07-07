#include "VulkanImGuiOverlay.h"

#include "Logger.h"
#include "VulkanBackend.h"
#include "VulkanMinecraftDemoProfiles.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>

#include "VulkanImGuiOverlayPresetControls.inl"
#include "VulkanImGuiOverlayPanels.inl"
#include "VulkanImGuiOverlayInitialize.inl"
#include "VulkanImGuiOverlayFrame.inl"
#include "VulkanImGuiOverlayLifecycle.inl"
