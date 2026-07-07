#include "HudRenderer.h"

#include <iomanip>
#include <sstream>

namespace
{
	/**
	 * Formats a vector with one decimal place for debug UI.
	 */
	std::string FormatVec3(const glm::vec3& value)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(1) << value.x << ", " << value.y << ", " << value.z;
		return stream.str();
	}

	/**
	 * Formats an integer vector for debug UI.
	 */
	std::string FormatIvec3(const glm::ivec3& value)
	{
		std::ostringstream stream;
		stream << value.x << ", " << value.y << ", " << value.z;
		return stream.str();
	}
}

namespace ve::ui
{
	void HudRenderer::DrawDebugOverlay(const HudFrameInfo& frame)
	{
		const std::string selected_placement_name(frame.block_registry.Get(frame.selected_placement_block).name);
		DrawText("Block " + selected_placement_name + "  1-9 hotbar", 10.0f, 10.0f, 1.2f);
		DrawText("LMB break  RMB place  Space jump  F fly  [] distance  F3 debug", 10.0f, 26.0f, 1.2f);
		if (!frame.show_debug_overlay)
		{
			return;
		}

		const glm::vec3 camera_position = frame.camera.GetPosition();
		DrawText(std::to_string(frame.displayed_fps) + " FPS", 10.0f, 50.0f, 1.6f);
		DrawText("XYZ " + FormatVec3(camera_position), 10.0f, 70.0f, 1.2f);
		DrawText(std::string("Mode ") + (frame.is_flying ? "fly" : "walk"), 10.0f, 86.0f, 1.2f);
		DrawText("Render distance " + std::to_string(frame.render_distance_chunks) + " chunks", 10.0f, 102.0f, 1.2f);
		DrawText("World events " + std::to_string(frame.pending_world_events), 10.0f, 118.0f, 1.2f);

		const std::string selected_target_text =
			frame.is_block_selected ? "Target " + FormatIvec3(frame.target_block) : "Target none";
		DrawText(selected_target_text, 10.0f, 134.0f, 1.2f);
	}
}
