#include <doctest/doctest.h>

#include "Logger.h"
#include "VulkanContextDetail.h"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
	struct LoggerReset
	{
		~LoggerReset()
		{
			try { ve::log::ResetRuntimeState(); }
			catch (...) {}
		}
	};
}

TEST_CASE("Vulkan debug callback reports validation messages through the no-throw logger")
{
	ve::log::ResetRuntimeState();
	ve::log::SetConsoleEnabled(false);
	std::vector<std::string> logs;
	LoggerReset reset;
	ve::log::SetCallback([&logs](std::string line) { logs.push_back(std::move(line)); });
	const VkDebugUtilsMessengerCreateInfoEXT info =
		ve::rendering::detail::CreateDebugMessengerInfo();
	REQUIRE(info.pfnUserCallback != nullptr);
	const VkDebugUtilsMessengerCallbackDataEXT data{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT,
		.pMessage = "synthetic validation warning"
	};

	CHECK(info.pfnUserCallback(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, &data, nullptr) == VK_FALSE);
	REQUIRE(logs.size() == 1);
	CHECK(logs.front().find("Vulkan validation: synthetic validation warning") != std::string::npos);

	ve::log::SetCallback([](std::string) { throw std::runtime_error("sink failure"); });
	CHECK_NOTHROW(static_cast<void>(info.pfnUserCallback(
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, nullptr, nullptr)));
}
