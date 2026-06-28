#include "voxel/Engine.h"

namespace voxel
{
	bool InputSnapshot::IsDown(Key key) const noexcept
	{
		switch (key)
		{
		case Key::W: return move_forward;
		case Key::A: return move_left;
		case Key::S: return move_back;
		case Key::D: return move_right;
		case Key::Space: return jump;
		case Key::Escape: return escape;
		case Key::F1: return f1;
		case Key::F2: return f2;
		case Key::Unknown:
		default: return false;
		}
	}

	RuntimeCommands& RuntimeCommands::SetBlock(BlockPosition position, Block block)
	{
		world_edits.push_back(WorldEdit{
			WorldEdit::Kind::SetBlock,
			position,
			{},
			block
		});
		return *this;
	}

	RuntimeCommands& RuntimeCommands::SetBlock(int x, int y, int z, Block block)
	{
		return SetBlock(At(x, y, z), block);
	}

	RuntimeCommands& RuntimeCommands::FillBox(BlockBox box, Block block)
	{
		world_edits.push_back(WorldEdit{
			WorldEdit::Kind::FillBox,
			{},
			box,
			block
		});
		return *this;
	}

	RuntimeCommands& RuntimeCommands::FillBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block)
	{
		return FillBox(Box(At(x1, y1, z1), At(x2, y2, z2)), block);
	}

	RuntimeCommands& RuntimeCommands::ClearBox(int x1, int y1, int z1, int x2, int y2, int z2)
	{
		return FillBox(x1, y1, z1, x2, y2, z2, Air);
	}

	RuntimeCommands& RuntimeCommands::RequestClose() noexcept
	{
		request_close = true;
		return *this;
	}
}
