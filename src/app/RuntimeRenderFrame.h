#pragma once

namespace ve::editor { class EditorRuntimeController; }
namespace ve::gameplay { struct RuntimeSettings; }
namespace ve::time { class FrameTimer; }

namespace ve::engine
{
	class GameController;
	class GameModel;
	class RuntimeInputActionTracker;
	class Window;

	struct RuntimeRenderFrame
	{
		RuntimeRenderFrame(Window& window,
			GameModel& model,
			GameController& controller,
			ve::editor::EditorRuntimeController& editor,
			RuntimeInputActionTracker& input_actions,
			ve::gameplay::RuntimeSettings& settings,
			const ve::time::FrameTimer& timer) noexcept
			: window(window), model(model), controller(controller), editor(editor),
			  input_actions(input_actions), settings(settings), timer(timer) {}

		Window& window;
		GameModel& model;
		GameController& controller;
		ve::editor::EditorRuntimeController& editor;
		RuntimeInputActionTracker& input_actions;
		ve::gameplay::RuntimeSettings& settings;
		const ve::time::FrameTimer& timer;
	};
}
