#include "StaticSceneSmokeScenarios.h"
#include "StaticSceneSmokeSupport.h"

#include <filesystem>

int main(int argc, char** argv)
{
	if (argc != 2 || argv == nullptr || argv[1] == nullptr) return 2;
	try
	{
		const static_scene_smoke::TriangleModelFixture model{};
		const std::filesystem::path assets{ argv[1] };
		int result = static_scene_smoke::RunBoundedScene(assets, model.Path());
		if (result == 0)
		{
			result = static_scene_smoke::RunExceptionRecovery(assets, model.Path());
		}
		if (result == 0)
		{
			result = static_scene_smoke::RunReentrantLifecycle(assets, model.Path());
		}
		return result;
	}
	catch (...) { return 3; }
}
