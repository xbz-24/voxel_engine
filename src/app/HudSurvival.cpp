#include "HudRenderer.h"

#include "Hotbar.h"
#include "Render2D.h"
#include "TextureLoader.h"

namespace ve::ui
{
	void HudRenderer::DrawIconRow(ve::rendering::TextureHandle texture, float row_start_x, float icon_y, float icon_size, float icon_spacing, int icon_count, float direction)
	{
		for (int icon_index = 0; icon_index < icon_count; icon_index++)
		{
			const float icon_x = row_start_x + (static_cast<float>(icon_index) * icon_spacing * direction);
			ve::rendering::DrawTexturedQuad(texture, icon_x, icon_y, icon_size, icon_size);
		}
	}

	void HudRenderer::DrawSurvivalHud(const ve::engine::Window& window, const ve::blocks::BlockRegistry& block_registry, ve::blocks::BlockId selected_placement_block)
	{
		const float window_width = static_cast<float>(window.GetWidth());
		const float window_height = static_cast<float>(window.GetHeight());
		const float window_center_x = window_width / 2.0f;
		const float window_center_y = window_height / 2.0f;
		const float crosshair_half_size = 30.0f;
		ve::rendering::DrawTexturedQuad(textures_.crosshair,
			window_center_x - crosshair_half_size,
			window_center_y - crosshair_half_size,
			crosshair_half_size * 2.0f,
			crosshair_half_size * 2.0f);

		const float scale = 3.0f;
		const float hotbar_width = 182.0f * scale;
		const float hotbar_height = 22.0f * scale;
		const float hotbar_x = (window_width / 2.0f) - (hotbar_width / 2.0f);
		const float hotbar_y = window_height - hotbar_height;
		ve::rendering::DrawTexturedQuad(textures_.hotbar, hotbar_x, hotbar_y, hotbar_width, hotbar_height);

		const float slot_size = 20.0f * scale;
		const float block_icon_size = 16.0f * scale;
		const float block_icon_inset = 3.0f * scale;
		const float selection_size = 24.0f * scale;
		const int selected_slot_index = ve::gameplay::HotbarIndexFor(selected_placement_block);
		ve::rendering::DrawTexturedQuad(textures_.hotbar_selection,
			hotbar_x + (static_cast<float>(selected_slot_index) * slot_size) - scale,
			hotbar_y - scale,
			selection_size,
			selection_size);

		const auto& hotbar_blocks = ve::gameplay::DefaultHotbarBlocks();
		for (int slot = 0; slot < ve::gameplay::HotbarSlotCount; slot++)
		{
			const ve::blocks::BlockId block = hotbar_blocks[static_cast<std::size_t>(slot)];
			const ve::rendering::TextureHandle block_texture =
				block_registry.TextureFor(block, ve::blocks::BlockFace::Top);
			ve::rendering::DrawTexturedQuad(block_texture,
				hotbar_x + (static_cast<float>(slot) * slot_size) + block_icon_inset,
				hotbar_y + block_icon_inset,
				block_icon_size,
				block_icon_size);
		}

		const float experience_width = 182.0f * scale;
		const float experience_height = 5.0f * scale;
		const float experience_x = (window_width / 2.0f) - (experience_width / 2.0f);
		const float experience_y = hotbar_y - experience_height - (2.0f * scale);
		ve::rendering::DrawTexturedQuad(textures_.experience_bar, experience_x, experience_y, experience_width, experience_height);

		const float icon_size = 9.0f * scale;
		const float icon_spacing = 8.0f * scale;
		const float icon_y = experience_y - icon_size - scale;
		DrawIconRow(textures_.heart, experience_x, icon_y, icon_size, icon_spacing, 10, 1.0f);
		DrawIconRow(textures_.hunger, experience_x + experience_width - icon_size, icon_y, icon_size, icon_spacing, 10, -1.0f);
	}
}
