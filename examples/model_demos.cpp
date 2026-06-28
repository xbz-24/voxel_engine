#include <voxel/Voxel.h>

#include <string_view>

namespace
{
	[[nodiscard]] bool IsColiseum(std::string_view name) noexcept
	{
		return name == "sponza" || name == "atrium" || name == "coliseo" || name == "coliseum";
	}
}

int main(int argc, char** argv)
{
	const std::string_view demo = argc > 1 ? std::string_view{ argv[1] } : std::string_view{ "aqua" };
	return IsColiseum(demo) ? voxel::RunSponzaAtriumDemo() : voxel::RunAquaModelDemo();
}
