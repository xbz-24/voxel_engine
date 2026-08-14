#include <voxel/Blocks.h>
#include <voxel/RuntimeCommands.h>

int main()
{
	voxel::RuntimeCommands commands{};
	commands.SetBlock(voxel::At(0, 64, 0), voxel::SeaLantern)
		.FillBox(voxel::At(-2, 63, -2), voxel::At(2, 63, 2), voxel::Stone)
		.ClearBox(voxel::At(-1, 64, -1), voxel::At(1, 66, 1))
		.RequestClose();

	return commands.world_edits.size() == 3 && commands.request_close ? 0 : 1;
}
