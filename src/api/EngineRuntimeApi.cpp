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

	bool InputSnapshot::IsActive(InputAction action) const noexcept
	{
		switch (action)
		{
		case InputAction::MoveForward: return move_forward;
		case InputAction::MoveLeft: return move_left;
		case InputAction::MoveBack: return move_back;
		case InputAction::MoveRight: return move_right;
		case InputAction::Jump: return jump;
		case InputAction::Cancel: return escape;
		case InputAction::ToggleDebugOverlay: return f1;
		case InputAction::ToggleRenderMode: return f2;
		case InputAction::PrimaryAction: return primary_action;
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

	RuntimeCommands& RuntimeCommands::SetBlock(int block_x, int block_y, int block_z, Block block)
	{
		return SetBlock(At(block_x, block_y, block_z), block);
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

	RuntimeCommands& RuntimeCommands::FillBox(BlockPosition first, BlockPosition second, Block block)
	{
		return FillBox(Box(first, second), block);
	}

	RuntimeCommands& RuntimeCommands::FillBox(
		int first_block_x,
		int first_block_y,
		int first_block_z,
		int second_block_x,
		int second_block_y,
		int second_block_z,
		Block block)
	{
		return FillBox(
			Box(
				At(first_block_x, first_block_y, first_block_z),
				At(second_block_x, second_block_y, second_block_z)),
			block);
	}

	RuntimeCommands& RuntimeCommands::ClearBox(BlockPosition first, BlockPosition second)
	{
		return FillBox(first, second, Air);
	}

	RuntimeCommands& RuntimeCommands::ClearBox(
		int first_block_x,
		int first_block_y,
		int first_block_z,
		int second_block_x,
		int second_block_y,
		int second_block_z)
	{
		return FillBox(
			first_block_x,
			first_block_y,
			first_block_z,
			second_block_x,
			second_block_y,
			second_block_z,
			Air);
	}

	RuntimeCommands& RuntimeCommands::RequestClose() noexcept
	{
		request_close = true;
		return *this;
	}
}
