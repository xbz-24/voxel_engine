#include "EditorDemoState.h"

namespace ve::editor
{
	/// Returns a stable UI label for a demo id.
	const char* DemoGameName(DemoGame demo) noexcept
	{
		switch (demo)
		{
		case DemoGame::MinecraftSandbox: return "Minecraft sandbox";
		case DemoGame::VoxelApiShowcase: return "Voxel API showcase";
		case DemoGame::EmptyPrototype: return "Empty prototype";
		}
		return "Unknown";
	}
}
