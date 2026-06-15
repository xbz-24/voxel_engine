#include <volk.h>

#include "VulkanSurface.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace ve::rendering
{
	/** Releases the surface. */
	VulkanSurface::~VulkanSurface() { Release(); }

	/** Creates a Vulkan surface from the GLFW window. */
	bool VulkanSurface::Create(VkInstance instance, Window& window)
	{
		Release();
		instance_ = instance;
		return glfwCreateWindowSurface(instance_, window.GetNativeWindow(), nullptr, &surface_) == VK_SUCCESS;
	}

	/** Destroys the Vulkan surface. */
	void VulkanSurface::Release()
	{
		if (surface_ != VK_NULL_HANDLE) vkDestroySurfaceKHR(instance_, surface_, nullptr);
		surface_ = VK_NULL_HANDLE;
		instance_ = VK_NULL_HANDLE;
	}

	/** Returns the Vulkan surface handle. */
	VkSurfaceKHR VulkanSurface::Handle() const noexcept { return surface_; }

	/** Returns the Vulkan-Hpp surface handle. */
	vk::SurfaceKHR VulkanSurface::CppHandle() const noexcept { return vk::SurfaceKHR{ surface_ }; }

	/** Reports whether the surface exists. */
	bool VulkanSurface::IsCreated() const noexcept { return surface_ != VK_NULL_HANDLE; }
}
