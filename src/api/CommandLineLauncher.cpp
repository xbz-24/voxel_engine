#include "CommandLineLauncher.h"

#include "CommandLineLauncherInternal.h"

#include <voxel/Engine.h>

#include <cstdlib>
#include <string>
#include <thread>
#include <utility>

namespace ve::launcher
{
	namespace
	{
		[[nodiscard]] std::string BuildDemoCommand(const DemoLaunchRequest& request)
		{
			std::string command = "start \"Voxel ";
			command += request.demo_name;
			command += "\" \"";
			command += request.executable;
			command += "\" --demo ";
			command += request.demo_name;
			if (request.smoke_frames > 0)
			{
				command += " --smoke-frames ";
				command += std::to_string(request.smoke_frames);
			}
			return command;
		}

		[[nodiscard]] int LaunchDemoProcess(const DemoLaunchRequest& request)
		{
			const std::string command = BuildDemoCommand(request);
			return std::system(command.c_str()) == 0 ? 0 : -1;
		}

		void ApplySmokeFrameLimiter(voxel::EngineConfig& config, int smoke_frames)
		{
			if (smoke_frames <= 0) return;
			config.HideDebugOverlay().OnUpdate([smoke_frames, frame_count = 0](voxel::FrameContext& frame) mutable {
				++frame_count;
				if (frame_count >= smoke_frames) frame.commands.RequestClose();
			});
		}

		[[nodiscard]] voxel::EngineConfig ConfigForDemo(std::string_view demo)
		{
			if (IsAquaDemo(demo)) return voxel::AquaModelDemo();
			if (IsSponzaDemo(demo)) return voxel::SponzaAtriumDemo();
			return voxel::DesertDemo();
		}

		[[nodiscard]] std::string_view ExecutablePath(const CommandLineArguments& arguments) noexcept
		{
			return arguments.empty() ? std::string_view{} : arguments.front();
		}

		void WaitAfterLaunch(const DemoProcess& demo)
		{
			if (demo.delay_after_launch.count() > 0) std::this_thread::sleep_for(demo.delay_after_launch);
		}

		[[nodiscard]] int LaunchDemoSuite(std::string_view executable, int smoke_frames)
		{
			int status = 0;
			for (const DemoProcess& demo : DemoSuite())
			{
				status |= LaunchDemoProcess(DemoLaunchRequest{ executable, demo.name, smoke_frames });
				WaitAfterLaunch(demo);
			}
			return status;
		}

		[[nodiscard]] int RunNamedDemo(std::string_view demo, int smoke_frames)
		{
			voxel::EngineConfig config = ConfigForDemo(demo);
			ApplySmokeFrameLimiter(config, smoke_frames);
			return voxel::Run(std::move(config));
		}
	}

	int RunCommandLine(const CommandLineArguments& arguments)
	{
		const LaunchParseResult parsed = ParseLaunchArguments(arguments);
		if (!parsed.ok) return -1;
		if (parsed.options.launch_all)
		{
			return LaunchDemoSuite(ExecutablePath(arguments), parsed.options.smoke_frames);
		}
		return RunNamedDemo(parsed.options.demo_name, parsed.options.smoke_frames);
	}
}
