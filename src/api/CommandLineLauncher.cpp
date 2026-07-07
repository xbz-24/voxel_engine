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
			command += request.requested_demo_name;
			command += "\" \"";
			command += request.executable_path;
			command += "\" --demo ";
			command += request.requested_demo_name;
			if (request.smoke_frame_limit > 0)
			{
				command += " --smoke-frames ";
				command += std::to_string(request.smoke_frame_limit);
			}
			return command;
		}

		[[nodiscard]] int LaunchDemoProcess(const DemoLaunchRequest& request)
		{
			const std::string command = BuildDemoCommand(request);
			return std::system(command.c_str()) == 0 ? 0 : -1;
		}

		void ApplySmokeFrameLimiter(voxel::EngineConfig& config, int smoke_frame_limit)
		{
			if (smoke_frame_limit <= 0) return;
			config.HideDebugOverlay().OnUpdate([smoke_frame_limit, rendered_frame_count = 0](voxel::FrameContext& frame) mutable {
				++rendered_frame_count;
				if (rendered_frame_count >= smoke_frame_limit) frame.commands.RequestClose();
			});
		}

		[[nodiscard]] voxel::EngineConfig ConfigForDemo(std::string_view requested_demo_name)
		{
			if (IsAquaDemoAlias(requested_demo_name)) return voxel::AquaModelDemo();
			if (IsSponzaDemoAlias(requested_demo_name)) return voxel::SponzaAtriumDemo();
			return voxel::DesertDemo();
		}

		[[nodiscard]] std::string_view ExecutablePath(const CommandLineArguments& arguments) noexcept
		{
			return arguments.empty() ? std::string_view{} : arguments.front();
		}

		void WaitAfterLaunch(const DemoProcess& demo_process)
		{
			if (demo_process.pause_after_launch.count() > 0) std::this_thread::sleep_for(demo_process.pause_after_launch);
		}

		[[nodiscard]] int LaunchDemoSuite(std::string_view executable_path, int smoke_frame_limit)
		{
			int accumulated_process_status = 0;
			for (const DemoProcess& demo_process : DemoSuite())
			{
				accumulated_process_status |= LaunchDemoProcess(DemoLaunchRequest{
					executable_path,
					demo_process.demo_name,
					smoke_frame_limit
				});
				WaitAfterLaunch(demo_process);
			}
			return accumulated_process_status;
		}

		[[nodiscard]] int RunNamedDemo(std::string_view requested_demo_name, int smoke_frame_limit)
		{
			voxel::EngineConfig config = ConfigForDemo(requested_demo_name);
			ApplySmokeFrameLimiter(config, smoke_frame_limit);
			return voxel::Run(std::move(config));
		}
	}

	int RunCommandLine(const CommandLineArguments& arguments)
	{
		const LaunchParseResult parsed = ParseLaunchArguments(arguments);
		if (!parsed.ok) return -1;
		if (parsed.options.launch_demo_suite)
		{
			return LaunchDemoSuite(ExecutablePath(arguments), parsed.options.smoke_frame_limit);
		}
		return RunNamedDemo(parsed.options.selected_demo_name, parsed.options.smoke_frame_limit);
	}
}
