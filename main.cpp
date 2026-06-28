#include "CommandLineLauncher.h"

#include <string_view>

namespace
{
	[[nodiscard]] ve::launcher::CommandLineArguments CaptureCommandLineArguments(int argc, char** argv)
	{
		ve::launcher::CommandLineArguments arguments{};
		if (argc <= 0 || argv == nullptr) return arguments;

		arguments.reserve(static_cast<ve::launcher::CommandLineArguments::size_type>(argc));
		for (int index = 0; index < argc; ++index)
		{
			arguments.emplace_back(argv[index] == nullptr ? std::string_view{} : std::string_view{ argv[index] });
		}

		return arguments;
	}
}

int main(int argc, char** argv)
{
	const ve::launcher::CommandLineArguments arguments = CaptureCommandLineArguments(argc, argv);
	return ve::launcher::RunCommandLine(arguments);
}
