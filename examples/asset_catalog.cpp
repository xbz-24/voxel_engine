#include <voxel/Assets.h>

#include <iostream>

int main()
{
	voxel::AssetCatalog assets{};
	assets.SearchRoot("assets")
		.Texture("terrain", "textures/terrain.png")
		.Model("player", voxel::AssetSource::Archive("game.pack", "models/player.glb"))
		.Sound("notification", voxel::AssetSource::Embedded({ 1U, 2U, 3U }));

	const auto issues = assets.Validate();
	for (const auto& issue : issues)
	{
		std::cerr << issue << '\n';
	}
	return issues.empty() ? 0 : 1;
}
