#include "CommandLineLauncher.h"

#include <voxel/Engine.h>

#include <array>
#include <chrono>
#include <cstdlib>
#include <string>
#include <string_view>
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

		struct DemoLaunchRequest
		{
			std::string_view executable;
			std::string_view demo_name;
			int smoke_frames = 0;
		};

		class PositiveIntegerParser final
		{
		public:
			[[nodiscard]] bool TryParse(std::string_view text, int& value) const noexcept
			{
				constexpr int kMaxFrameCount = 100000;

				if (text.empty()) return false;

				int parsed = 0;
				for (const char ch : text)
				{
					if (ch < '0' || ch > '9') return false;

					const int digit = ch - '0';
					if (parsed > (kMaxFrameCount - digit) / 10) return false;
					parsed = (parsed * 10) + digit;
				}

				if (parsed <= 0) return false;
				value = parsed;
				return true;
			}
		};

		class DemoCatalog final
		{
		public:
			[[nodiscard]] const std::array<DemoProcess, 3>& Suite() const noexcept
			{
				static constexpr std::array<DemoProcess, 3> kDemoSuite{ {
					DemoProcess{ "aqua", std::chrono::seconds{ 5 } },
					DemoProcess{ "sponza", std::chrono::seconds{ 8 } },
					DemoProcess{ "voxeldemo", std::chrono::seconds{ 0 } }
				} };
				return kDemoSuite;
			}

			[[nodiscard]] bool IsSuiteAlias(std::string_view demo) const noexcept
			{
				return demo == "all" || demo == "all-demos" || demo == "demo-all";
			}

			[[nodiscard]] voxel::EngineConfig CreateConfig(std::string_view demo) const
			{
				if (IsAquaDemo(demo)) return voxel::AquaModelDemo();
				if (IsSponzaDemo(demo)) return voxel::SponzaAtriumDemo();
				return voxel::DesertDemo();
			}

		private:
			[[nodiscard]] bool IsAquaDemo(std::string_view demo) const noexcept
			{
				return demo == "aqua" || demo == "konosuba" || demo == "konosuba-aqua";
			}

			[[nodiscard]] bool IsSponzaDemo(std::string_view demo) const noexcept
			{
				return demo == "sponza" || demo == "atrium" || demo == "sponza-atrium" ||
					demo == "coliseo" || demo == "coliseum";
			}
		};

		class SmokeFrameLimiter final
		{
		public:
			void Apply(voxel::EngineConfig& config, int smoke_frames) const
			{
				if (smoke_frames <= 0) return;

				config.HideDebugOverlay().OnUpdate([smoke_frames, frame_count = 0](voxel::FrameContext& frame) mutable {
					++frame_count;
					if (frame_count >= smoke_frames) frame.commands.RequestClose();
				});
			}
		};

		class DemoProcessLauncher final
		{
		public:
			[[nodiscard]] int Launch(const DemoLaunchRequest& request) const
			{
				const std::string command = BuildCommand(request);
				return std::system(command.c_str()) == 0 ? 0 : -1;
			}

		private:
			[[nodiscard]] std::string BuildCommand(const DemoLaunchRequest& request) const
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
		};

		class LaunchArgumentParser final
		{
		public:
			explicit LaunchArgumentParser(const DemoCatalog& demos) noexcept : demos_{ demos } {}

			[[nodiscard]] LaunchParseResult Parse(const CommandLineArguments& arguments) const noexcept
			{
				LaunchParseResult result{};
				result.options.launch_all = arguments.size() == 1;

				for (CommandLineArguments::size_type index = 1; index < arguments.size(); ++index)
				{
					const std::string_view arg = arguments[index];

					if (arg == "--demo")
					{
						if (!ApplyDemoOption(arguments, index, result)) return Invalid(result);
					}
					else if (arg == "--smoke-frames")
					{
						if (!ApplySmokeFramesOption(arguments, index, result)) return Invalid(result);
					}
					else if (arg == "--all-demos")
					{
						result.options.launch_all = true;
					}
					else
					{
						return Invalid(result);
					}
				}

				return result;
			}

		private:
			[[nodiscard]] bool ApplyDemoOption(const CommandLineArguments& arguments,
				CommandLineArguments::size_type& index,
				LaunchParseResult& result) const noexcept
			{
				std::string_view demo{};
				if (!TryReadValue(arguments, index, demo)) return false;

				result.options.demo_name = demo;
				result.options.launch_all = demos_.IsSuiteAlias(demo);
				return true;
			}

			[[nodiscard]] bool ApplySmokeFramesOption(const CommandLineArguments& arguments,
				CommandLineArguments::size_type& index,
				LaunchParseResult& result) const noexcept
			{
				std::string_view smoke_frames{};
				if (!TryReadValue(arguments, index, smoke_frames)) return false;
				return positive_ints_.TryParse(smoke_frames, result.options.smoke_frames);
			}

			[[nodiscard]] static LaunchParseResult Invalid(LaunchParseResult result) noexcept
			{
				result.ok = false;
				return result;
			}

			[[nodiscard]] static bool TryReadValue(const CommandLineArguments& arguments,
				CommandLineArguments::size_type& index,
				std::string_view& value) noexcept
			{
				if (index + 1 >= arguments.size()) return false;

				++index;
				value = arguments[index];
				return true;
			}

			const DemoCatalog& demos_;
			PositiveIntegerParser positive_ints_{};
		};

		class CommandLineRunner final
		{
		public:
			[[nodiscard]] int Run(const CommandLineArguments& arguments) const
			{
				const LaunchParseResult parsed = LaunchArgumentParser{ demos_ }.Parse(arguments);
				if (!parsed.ok) return -1;

				if (parsed.options.launch_all)
				{
					return LaunchDemoSuite(ExecutablePath(arguments), parsed.options.smoke_frames);
				}

				return RunNamedDemo(parsed.options.demo_name, parsed.options.smoke_frames);
			}

		private:
			[[nodiscard]] int LaunchDemoSuite(std::string_view executable, int smoke_frames) const
			{
				int status = 0;
				for (const DemoProcess demo : demos_.Suite())
				{
					status |= process_launcher_.Launch(DemoLaunchRequest{ executable, demo.name, smoke_frames });
					WaitAfterLaunch(demo);
				}
				return status;
			}

			[[nodiscard]] int RunNamedDemo(std::string_view demo, int smoke_frames) const
			{
				voxel::EngineConfig config = demos_.CreateConfig(demo);
				smoke_limiter_.Apply(config, smoke_frames);
				return voxel::Run(std::move(config));
			}

			static void WaitAfterLaunch(const DemoProcess& demo)
			{
				if (demo.delay_after_launch.count() > 0)
				{
					std::this_thread::sleep_for(demo.delay_after_launch);
				}
			}

			[[nodiscard]] static std::string_view ExecutablePath(const CommandLineArguments& arguments) noexcept
			{
				return arguments.empty() ? std::string_view{} : arguments.front();
			}

			DemoCatalog demos_{};
			DemoProcessLauncher process_launcher_{};
			SmokeFrameLimiter smoke_limiter_{};
		};
	}

	LaunchParseResult ParseLaunchArguments(const CommandLineArguments& arguments) noexcept
	{
		const DemoCatalog demos{};
		return LaunchArgumentParser{ demos }.Parse(arguments);
	}

	int RunCommandLine(const CommandLineArguments& arguments)
	{
		return CommandLineRunner{}.Run(arguments);
	}
}
