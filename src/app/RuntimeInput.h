#pragma once

#include "EngineConfiguration.h"

#include <bitset>
#include <cstddef>

struct GLFWwindow;

namespace ve::engine
{
	enum class RuntimeInputAction
	{
		MoveForward,
		MoveLeft,
		MoveBack,
		MoveRight,
		Jump,
		Cancel,
		ToggleDebugOverlay,
		ToggleRenderMode,
		PrimaryAction
	};

	[[nodiscard]] RuntimeInputSnapshot CaptureRuntimeInputSnapshot(GLFWwindow* native_window) noexcept;
	[[nodiscard]] bool IsRuntimeInputActionActive(
		const RuntimeInputSnapshot& runtime_input_snapshot,
		RuntimeInputAction runtime_input_action) noexcept;

	class RuntimeInputActionTracker
	{
	public:
		void Update(const RuntimeInputSnapshot& runtime_input_snapshot) noexcept;
		[[nodiscard]] bool IsDown(RuntimeInputAction runtime_input_action) const noexcept;
		[[nodiscard]] bool WasJustPressed(RuntimeInputAction runtime_input_action) const noexcept;

	private:
		static constexpr std::size_t TrackedRuntimeInputActionCount = 9;

		std::bitset<TrackedRuntimeInputActionCount> current_action_states_;
		std::bitset<TrackedRuntimeInputActionCount> previous_action_states_;
	};
}
