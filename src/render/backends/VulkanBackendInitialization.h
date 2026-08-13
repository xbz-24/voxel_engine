#pragma once

#include <string>
#include <utility>

namespace ve::rendering
{
	enum class VulkanBackendInitializationFailure
	{
		None,
		ContextCreationFailed,
		SurfaceCreationFailed,
		PhysicalDeviceSelectionFailed,
		LogicalDeviceCreationFailed,
		AllocatorCreationFailed,
		SwapchainCreationFailed
	};

	struct VulkanBackendInitializationResult
	{
		VulkanBackendInitializationFailure failure = VulkanBackendInitializationFailure::None;
		std::string message;

		[[nodiscard]] static VulkanBackendInitializationResult Success()
		{
			return {};
		}

		[[nodiscard]] static VulkanBackendInitializationResult Failure(
			VulkanBackendInitializationFailure failure,
			std::string message)
		{
			return { failure, std::move(message) };
		}

		[[nodiscard]] explicit operator bool() const noexcept
		{
			return failure == VulkanBackendInitializationFailure::None;
		}
	};
}
