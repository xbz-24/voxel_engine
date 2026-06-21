#pragma once

#include <volk.h>
#include <vulkan/vulkan.hpp>

namespace ve::engine { class Window; }

namespace ve::rendering
{
	/** Owns the Vulkan presentation surface created from the engine window. */
	class VulkanSurface
	{
	public:
		/** Releases the surface when the wrapper is destroyed. */
		~VulkanSurface();

		/** @param instance Vulkan instance. @param window Engine window used for presentation. @return True when surface creation succeeds. */
		[[nodiscard]] bool Create(VkInstance instance, ve::engine::Window& window);

		/** Destroys the surface and resets the handle. */
		void Release();

		/** @return Vulkan surface handle, or VK_NULL_HANDLE when inactive. */
		[[nodiscard]] VkSurfaceKHR Handle() const noexcept;

		/** @return C++ Vulkan-Hpp surface handle, or an empty handle when inactive. */
		[[nodiscard]] vk::SurfaceKHR CppHandle() const noexcept;

		/** @return True when a surface handle is active. */
		[[nodiscard]] bool IsCreated() const noexcept;

	private:
		VkInstance instance_ = VK_NULL_HANDLE;
		VkSurfaceKHR surface_ = VK_NULL_HANDLE;
	};
}
