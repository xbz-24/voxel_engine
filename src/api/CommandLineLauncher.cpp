#include "CommandLineLauncher.h"

#include <voxel/Engine.h>

#include <array>
#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>
#include <utility>

namespace ve::launcher
{
	namespace
	{
		struct DemoProcess
		{
			std::string_view name;
			std::chrono::seconds delay_after_launch;
		};

		constexpr std::array kDemoSuite{
			DemoProcess{ "aqua", std::chrono::seconds{ 5 } },
			DemoProcess{ "sponza", std::chrono::seconds{ 8 } },
			DemoProcess{ "voxeldemo", std::chrono::seconds{ 0 } }
		};

		[[nodiscard]] bool TryParsePositiveInt(std::string_view text, int& value) noexcept
		{
			if (text.empty()) return false;
			int parsed = 0;
			for (const char ch : text)
			{
				if (ch < '0' || ch > '9') return false;
				parsed = (parsed * 10) + (ch - '0');
				if (parsed > 100000) return false;
			}
			value = parsed;
			return value > 0;
		}

		[[nodiscard]] bool IsAllDemosAlias(std::string_view demo) noexcept
		{
			return demo == "all" || demo == "all-demos" || demo == "demo-all";
		}

		[[nodiscard]] voxel::EngineConfig DemoConfig(std::string_view demo)
		{
			if (demo == "aqua" || demo == "konosuba" || demo == "konosuba-aqua") return voxel::AquaModelDemo();
			if (demo == "sponza" || demo == "atrium" || demo == "sponza-atrium" ||
				demo == "coliseo" || demo == "coliseum") return voxel::SponzaAtriumDemo();
			return voxel::DesertDemo();
		}

		void ApplySmokeFrameLimit(voxel::EngineConfig& config, int smoke_frames)
		{
			if (smoke_frames <= 0) return;
			config.HideDebugOverlay().OnUpdate([smoke_frames, frame_count = 0](voxel::FrameContext& frame) mutable {
				++frame_count;
				if (frame_count >= smoke_frames) frame.commands.RequestClose();
			});
		}

		[[nodiscard]] std::string BuildProcessCommand(std::string_view executable,
			std::string_view demo,
			int smoke_frames)
		{
			std::string command = "start \"Voxel ";
			command += demo;
			command += "\" \"";
			command += executable;
			command += "\" --demo ";
			command += demo;
			if (smoke_frames > 0)
			{
				command += " --smoke-frames ";
				command += std::to_string(smoke_frames);
			}
			return command;
		}

		[[nodiscard]] int LaunchDemoProcess(std::string_view executable,
			std::string_view demo,
			int smoke_frames)
		{
			const std::string command = BuildProcessCommand(executable, demo, smoke_frames);
			return std::system(command.c_str()) == 0 ? 0 : -1;
		}

		[[nodiscard]] int LaunchDemoSuite(std::string_view executable, int smoke_frames)
		{
			int status = 0;
			for (const DemoProcess demo : kDemoSuite)
			{
				status |= LaunchDemoProcess(executable, demo.name, smoke_frames);
				if (demo.delay_after_launch.count() > 0)
				{
					std::this_thread::sleep_for(demo.delay_after_launch);
				}
			}
			return status;
		}

		[[nodiscard]] int RunNamedDemo(std::string_view demo, int smoke_frames)
		{
			voxel::EngineConfig config = DemoConfig(demo);
			ApplySmokeFrameLimit(config, smoke_frames);
			return voxel::Run(std::move(config));
		}
	}

	LaunchParseResult ParseLaunchArguments(int argc, char** argv) noexcept
	{
		LaunchParseResult result{};
		result.options.launch_all = argc == 1;

		for (int index = 1; index < argc; ++index)
		{
			const std::string_view arg = argv[index];
			if (arg == "--demo" && index + 1 < argc)
			{
				result.options.demo_name = argv[++index];
				result.options.launch_all = IsAllDemosAlias(result.options.demo_name);
			}
			else if (arg == "--smoke-frames" && index + 1 < argc)
			{
				if (!TryParsePositiveInt(argv[++index], result.options.smoke_frames))
				{
					result.ok = false;
					return result;
				}
			}
			else if (arg == "--all-demos")
			{
				result.options.launch_all = true;
			}
			else
			{
				result.ok = false;
				return result;
			}
		}
		return result;
	}

	int RunCommandLine(int argc, char** argv)
	{
		const LaunchParseResult parsed = ParseLaunchArguments(argc, argv);
		if (!parsed.ok) return -1;
		if (parsed.options.launch_all) return LaunchDemoSuite(argv[0], parsed.options.smoke_frames);
		return RunNamedDemo(parsed.options.demo_name, parsed.options.smoke_frames);
	}
}
