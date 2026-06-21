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
		const std::string selectedPlacementName(frame.blockRegistry.Get(frame.selectedPlacementBlock).name);
		DrawText("Block " + selectedPlacementName + "  1-9 hotbar", 10.0f, 10.0f, 1.2f);
		DrawText("LMB break  RMB place  Space jump  F fly  [] distance  F3 debug", 10.0f, 26.0f, 1.2f);
		if (!frame.showDebugOverlay)
		{
			return;
		}

		const glm::vec3 cameraPosition = frame.camera.GetPosition();
		DrawText(std::to_string(frame.displayedFps) + " FPS", 10.0f, 50.0f, 1.6f);
		DrawText("XYZ " + FormatVec3(cameraPosition), 10.0f, 70.0f, 1.2f);
		DrawText(std::string("Mode ") + (frame.isFlying ? "fly" : "walk"), 10.0f, 86.0f, 1.2f);
		DrawText("Render distance " + std::to_string(frame.renderDistanceChunks) + " chunks", 10.0f, 102.0f, 1.2f);
		DrawText("World events " + std::to_string(frame.pendingWorldEvents), 10.0f, 118.0f, 1.2f);

		const std::string selectedText = frame.isBlockSelected ? "Target " + FormatIvec3(frame.targetBlock) : "Target none";
		DrawText(selectedText, 10.0f, 134.0f, 1.2f);
	}
}
