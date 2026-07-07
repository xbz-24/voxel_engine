#include <voxel/Voxel.h>

#include <string_view>
#include <vector>

namespace
{
	using CommandLineArguments = std::vector<std::string_view>;

	[[nodiscard]] CommandLineArguments CaptureCommandLineArguments(int argc, char** argv)
	{
		CommandLineArguments arguments{};
		if (argc <= 0 || argv == nullptr) return arguments;

		arguments.reserve(static_cast<CommandLineArguments::size_type>(argc));
		for (int index = 0; index < argc; ++index)
		{
			arguments.emplace_back(argv[index] == nullptr ? std::string_view{} : std::string_view{ argv[index] });
		}

		return arguments;
	}

	[[nodiscard]] std::string_view SelectedDemo(const CommandLineArguments& arguments) noexcept
	{
		return arguments.size() > 1 ? arguments[1] : std::string_view{ "aqua" };
	}

	[[nodiscard]] bool IsColiseum(std::string_view name) noexcept
	{
		return name == "sponza" || name == "atrium" || name == "coliseo" || name == "coliseum";
	}
}

int main(int argc, char** argv)
{
	const CommandLineArguments arguments = CaptureCommandLineArguments(argc, argv);
	const std::string_view demo = SelectedDemo(arguments);
	return IsColiseum(demo) ? voxel::RunSponzaAtriumDemo() : voxel::RunAquaModelDemo();
}
