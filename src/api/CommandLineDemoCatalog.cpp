#include "CommandLineLauncherInternal.h"

#include <array>

namespace ve::launcher
{
	std::span<const DemoProcess> DemoSuite() noexcept
	{
		static constexpr std::array<DemoProcess, 3> kDemoSuite{ {
			DemoProcess{ "aqua", std::chrono::seconds{ 5 } },
			DemoProcess{ "sponza", std::chrono::seconds{ 8 } },
			DemoProcess{ "voxeldemo", std::chrono::seconds{ 0 } }
		} };
		return kDemoSuite;
	}

	bool IsAquaDemoAlias(std::string_view requested_demo_name) noexcept
	{
		return requested_demo_name == "aqua" || requested_demo_name == "konosuba" ||
			requested_demo_name == "konosuba-aqua";
	}

	bool IsSponzaDemoAlias(std::string_view requested_demo_name) noexcept
	{
		return requested_demo_name == "sponza" || requested_demo_name == "atrium" ||
			requested_demo_name == "sponza-atrium" || requested_demo_name == "coliseo" ||
			requested_demo_name == "coliseum";
	}

	bool IsDemoSuiteAlias(std::string_view requested_demo_name) noexcept
	{
		return requested_demo_name == "all" || requested_demo_name == "all-demos" ||
			requested_demo_name == "demo-all";
	}
}
