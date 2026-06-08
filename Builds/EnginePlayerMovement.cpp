#include "Engine.h"

#include "Input.h"
#include "PlayerMovementInput.h"

#include <algorithm>

namespace
{
	/**
	 * Toggles fly mode from the gameplay shortcut key.
	 */
	void ConsumeFlyToggle(GLFWwindow* window, ve::gameplay::RuntimeSettings& settings, bool& wasPressed)
	{
		if (!ve::input::WasPressed(window, ve::input::Key::F, wasPressed))
		{
			return;
		}
		settings.isFlying = !settings.isFlying;
		settings.verticalVelocity = 0.0f;
	}

	/**
	 * Adjusts render distance from a one-shot shortcut key.
	 */
	void ConsumeRenderDistanceAdjustment(GLFWwindow* window, ve::gameplay::RuntimeSettings& settings, ve::input::Key key, int amount, bool& wasPressed)
	{
		if (ve::input::WasPressed(window, key, wasPressed))
		{
			settings.renderDistanceChunks = std::clamp(settings.renderDistanceChunks + amount, 1, 6);
		}
	}
}

/// Processes player movement input unless the settings menu owns input.
void Engine::ProcessInput(Window& window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameTimeDeltaSeconds)
{
	const bool wasMenuOpen = _runtimeSettings.isSettingsMenuOpen;
	_settingsMenuController.ProcessInput(window, _runtimeSettings);
	if (wasMenuOpen || _runtimeSettings.isSettingsMenuOpen)
	{
		return;
	}

	handlePlayerMovementAndWindowInput(window.GetNativeWindow(), world, blockRegistry, camera, frameTimeDeltaSeconds);
}

/// Reads gameplay shortcuts, moves the camera, and advances player physics.
void Engine::handlePlayerMovementAndWindowInput(GLFWwindow* window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds)
{
	ConsumeFlyToggle(window, _runtimeSettings, _wasFlyTogglePressed);
	ConsumeRenderDistanceAdjustment(window, _runtimeSettings, ve::input::Key::LeftBracket, -1, _wasRenderDistanceDecreasePressed);
	ConsumeRenderDistanceAdjustment(window, _runtimeSettings, ve::input::Key::RightBracket, 1, _wasRenderDistanceIncreasePressed);
	const float speed = 5.0f * static_cast<float>(frameDeltaTimeSeconds);
	const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(window);
	ve::gameplay::ApplyPlanarMovement(intent, camera, speed);
	if (_runtimeSettings.isFlying)
	{
		ve::gameplay::ApplyFlyingMovement(intent, camera, speed);
	}
	else if (_isGrounded && ve::gameplay::WantsJump(intent))
	{
		_runtimeSettings.verticalVelocity = 7.0f;
	}
	ApplyPlayerPhysics(world, blockRegistry, camera, frameDeltaTimeSeconds);
}
