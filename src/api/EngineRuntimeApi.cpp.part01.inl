#include "voxel/Engine.h"

#include <utility>

namespace voxel
{
	EngineStartResult EngineStartResult::Success()
	{
		return {};
	}

	EngineStartResult EngineStartResult::InvalidConfiguration(std::vector<std::string> issues)
	{
		return EngineStartResult{
			EngineStartFailure::InvalidConfiguration,
			std::move(issues),
			"EngineConfig is invalid"
		};
	}

	EngineStartResult EngineStartResult::RuntimeUnavailable(std::string message)
	{
		return EngineStartResult{
			EngineStartFailure::RuntimeUnavailable,
			{},
			std::move(message)
		};
	}

	EngineStartResult EngineStartResult::RuntimeStartupFailed(std::string message)
	{
		return EngineStartResult{
			EngineStartFailure::RuntimeStartupFailed,
			{},
			std::move(message)
		};
	}

	EngineStartResult::operator bool() const noexcept
	{
		return failure == EngineStartFailure::None;
	}

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

