#include "GameController.h"

#include "BlockInteraction.h"
#include "BlockRegistry.h"
#include "BlockRaycaster.h"
#include "CoreTypes.h"
#include "GameplayInput.h"
#include "Hotbar.h"
#include "Input.h"
#include "PlayerMovementInput.h"
#include "RenderDistanceSettings.h"
#include "WorldGridMath.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>

/// Updates generic Vulkan gameplay and asynchronous terrain generation.
	void GameController::UpdateVulkanWorld(Window& window,
		GameModel& model,
		const ve::blocks::BlockRegistry& block_registry,
		ve::gameplay::RuntimeSettings&,
		double delta_seconds,
		bool ui_captures_input)
	{
		const ve::input::InputSnapshot input = ve::input::CaptureInputSnapshot(window);
		if (ve::input::IsPressed(input, ve::input::Key::Escape)) window.Close();
		(void)model.PumpAsyncWorldGeneration();
		// VulkanFrameOrchestrator builds its own world mesh; the legacy chunk mesh pipeline uploads OpenGL buffers.
		const float speed = 18.0f * ve::core::ToFloat(std::max(delta_seconds, 0.001));
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(input);
		if (!ui_captures_input)
		{
			ve::gameplay::ApplyPlanarMovement(intent, model.MutableCamera(), speed);
			ve::gameplay::ApplyFlyingMovement(intent, model.MutableCamera(), speed);
		}
		if (!ui_captures_input &&
			ve::gameplay::ConsumeBlockBreak(input, input_state_.was_left_mouse_button_pressed))
		{
			const std::optional<ve::gameplay::BlockRaycastHit> hit =
				ve::gameplay::RaycastBlocks(model.GetWorld(), block_registry, model.MutableCamera().GetPosition(), model.MutableCamera().GetForward(), 18.0f);
			if (hit)
			{
				const ve::blocks::BlockId target = model.GetWorld().GetBlock(hit->targetBlock);
				if (!IsTunnelMaterial(target) || !CarveRoundedHole(model.MutableWorld(), block_registry, hit->targetBlock))
				{
					ve::gameplay::BreakBlock(model.MutableWorld(), hit->targetBlock);
				}
			}
		}
	}
}