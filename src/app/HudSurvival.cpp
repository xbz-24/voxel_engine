#include "HudRenderer.h"

#include "Hotbar.h"
#include "Render2D.h"
#include "TextureLoader.h"

namespace ve::ui
{
	void HudRenderer::DrawIconRow(GLuint texture, float startX, float y, float iconSize, float spacing, int count, float direction)
	{
		for (int i = 0; i < count; i++)
		{
			const float iconX = startX + (static_cast<float>(i) * spacing * direction);
			ve::rendering::DrawTexturedQuad(texture, iconX, y, iconSize, iconSize);
		}
	}

	void HudRenderer::DrawSurvivalHud(const ve::engine::Window& window, const ve::blocks::BlockRegistry& blockRegistry, ve::blocks::BlockId selectedPlacementBlock)
	{
		const float windowWidth = static_cast<float>(window.GetWidth());
		const float windowHeight = static_cast<float>(window.GetHeight());
		const float centerX = windowWidth / 2.0f;
		const float centerY = windowHeight / 2.0f;
		const float crosshairHalfSize = 30.0f;
		ve::rendering::DrawTexturedQuad(_textures.crosshair, centerX - crosshairHalfSize, centerY - crosshairHalfSize, crosshairHalfSize * 2.0f, crosshairHalfSize * 2.0f);

		const float scale = 3.0f;
		const float hotbarWidth = 182.0f * scale;
		const float hotbarHeight = 22.0f * scale;
		const float hotbarX = (windowWidth / 2.0f) - (hotbarWidth / 2.0f);
		const float hotbarY = windowHeight - hotbarHeight;
		ve::rendering::DrawTexturedQuad(_textures.hotbar, hotbarX, hotbarY, hotbarWidth, hotbarHeight);

		const float slotSize = 20.0f * scale;
		const float blockIconSize = 16.0f * scale;
		const float blockIconInset = 3.0f * scale;
		const float selectionSize = 24.0f * scale;
		const int selectedSlot = ve::gameplay::HotbarIndexFor(selectedPlacementBlock);
		ve::rendering::DrawTexturedQuad(_textures.hotbarSelection, hotbarX + (static_cast<float>(selectedSlot) * slotSize) - scale, hotbarY - scale, selectionSize, selectionSize);

		const auto& hotbarBlocks = ve::gameplay::DefaultHotbarBlocks();
		for (int slot = 0; slot < ve::gameplay::HotbarSlotCount; slot++)
		{
			const ve::blocks::BlockId blockId = hotbarBlocks[static_cast<std::size_t>(slot)];
			ve::rendering::DrawTexturedQuad(ve::rendering::NativeOpenGLTexture(blockRegistry.TextureFor(blockId, ve::blocks::BlockFace::Top)), hotbarX + (static_cast<float>(slot) * slotSize) + blockIconInset, hotbarY + blockIconInset, blockIconSize, blockIconSize);
		}

		const float experienceWidth = 182.0f * scale;
		const float experienceHeight = 5.0f * scale;
		const float experienceX = (windowWidth / 2.0f) - (experienceWidth / 2.0f);
		const float experienceY = hotbarY - experienceHeight - (2.0f * scale);
		ve::rendering::DrawTexturedQuad(_textures.experienceBar, experienceX, experienceY, experienceWidth, experienceHeight);

		const float iconSize = 9.0f * scale;
		const float iconSpacing = 8.0f * scale;
		const float iconsY = experienceY - iconSize - scale;
		DrawIconRow(_textures.heart, experienceX, iconsY, iconSize, iconSpacing, 10, 1.0f);
		DrawIconRow(_textures.hunger, experienceX + experienceWidth - iconSize, iconsY, iconSize, iconSpacing, 10, -1.0f);
	}
}
