#include "RuntimeInput.h"

#include "Input.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>
#include <optional>

namespace ve::engine
{
	namespace
	{
		constexpr std::array<RuntimeInputAction, 9> kTrackedRuntimeInputActions{ {
			RuntimeInputAction::MoveForward,
			RuntimeInputAction::MoveLeft,
			RuntimeInputAction::MoveBack,
			RuntimeInputAction::MoveRight,
			RuntimeInputAction::Jump,
			RuntimeInputAction::Cancel,
			RuntimeInputAction::ToggleDebugOverlay,
			RuntimeInputAction::ToggleRenderMode,
			RuntimeInputAction::PrimaryAction
		} };

		[[nodiscard]] std::optional<std::size_t> RuntimeInputActionIndex(
			RuntimeInputAction runtime_input_action) noexcept
		{
			for (std::size_t action_index = 0; action_index < kTrackedRuntimeInputActions.size(); ++action_index)
			{
				if (kTrackedRuntimeInputActions[action_index] == runtime_input_action) return action_index;
			}
			return std::nullopt;
		}
	}

	RuntimeInputSnapshot CaptureRuntimeInputSnapshot(GLFWwindow* native_window) noexcept
	{
		const ve::input::InputSnapshot central_input_snapshot =
			ve::input::CaptureInputSnapshot(native_window);
		RuntimeInputSnapshot runtime_input_snapshot{};
		runtime_input_snapshot.move_forward =
			ve::input::IsPressed(central_input_snapshot, ve::input::Key::W);
		runtime_input_snapshot.move_left =
			ve::input::IsPressed(central_input_snapshot, ve::input::Key::A);
		runtime_input_snapshot.move_back =
			ve::input::IsPressed(central_input_snapshot, ve::input::Key::S);
		runtime_input_snapshot.move_right =
			ve::input::IsPressed(central_input_snapshot, ve::input::Key::D);
		runtime_input_snapshot.jump =
			ve::input::IsPressed(central_input_snapshot, ve::input::Key::Space);
		runtime_input_snapshot.escape =
			ve::input::IsPressed(central_input_snapshot, ve::input::Key::Escape);
		runtime_input_snapshot.f1 =
			ve::input::IsPressed(central_input_snapshot, ve::input::Key::F1);
		runtime_input_snapshot.f2 =
			ve::input::IsPressed(central_input_snapshot, ve::input::Key::F2);
		runtime_input_snapshot.primary_action =
			ve::input::IsPressed(central_input_snapshot, ve::input::MouseButton::Left);

		if (native_window != nullptr)
		{
			glfwGetCursorPos(
				native_window,
				&runtime_input_snapshot.mouse_x,
				&runtime_input_snapshot.mouse_y);
		}

		return runtime_input_snapshot;
	}

	bool IsRuntimeInputActionActive(
		const RuntimeInputSnapshot& runtime_input_snapshot,
		RuntimeInputAction runtime_input_action) noexcept
	{
		switch (runtime_input_action)
		{
		case RuntimeInputAction::MoveForward: return runtime_input_snapshot.move_forward;
		case RuntimeInputAction::MoveLeft: return runtime_input_snapshot.move_left;
		case RuntimeInputAction::MoveBack: return runtime_input_snapshot.move_back;
		case RuntimeInputAction::MoveRight: return runtime_input_snapshot.move_right;
		case RuntimeInputAction::Jump: return runtime_input_snapshot.jump;
		case RuntimeInputAction::Cancel: return runtime_input_snapshot.escape;
		case RuntimeInputAction::ToggleDebugOverlay: return runtime_input_snapshot.f1;
		case RuntimeInputAction::ToggleRenderMode: return runtime_input_snapshot.f2;
		case RuntimeInputAction::PrimaryAction: return runtime_input_snapshot.primary_action;
		default: return false;
		}
	}

	void RuntimeInputActionTracker::Update(const RuntimeInputSnapshot& runtime_input_snapshot) noexcept
	{
		previous_action_states_ = current_action_states_;
		current_action_states_.reset();

		for (std::size_t action_index = 0; action_index < kTrackedRuntimeInputActions.size(); ++action_index)
		{
			const RuntimeInputAction tracked_runtime_input_action =
				kTrackedRuntimeInputActions[action_index];
			current_action_states_.set(
				action_index,
				IsRuntimeInputActionActive(runtime_input_snapshot, tracked_runtime_input_action));
		}
	}

	bool RuntimeInputActionTracker::IsDown(RuntimeInputAction runtime_input_action) const noexcept
	{
		const std::optional<std::size_t> action_index = RuntimeInputActionIndex(runtime_input_action);
		return action_index.has_value() && current_action_states_[*action_index];
	}

	bool RuntimeInputActionTracker::WasJustPressed(RuntimeInputAction runtime_input_action) const noexcept
	{
		const std::optional<std::size_t> action_index = RuntimeInputActionIndex(runtime_input_action);
		return action_index.has_value() &&
			current_action_states_[*action_index] &&
			!previous_action_states_[*action_index];
	}
}
