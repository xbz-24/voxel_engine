#pragma once

#include "RenderCommandExecutor.h"

#include <array>
#include <string>

namespace ve::rendering
{
	enum class GraphicsCommandLayer : std::size_t
	{
		World = 0,
		Hud,
		Editor,
		PluginOverlay,
		Count
	};

	inline constexpr std::size_t kGraphicsCommandLayerCount = static_cast<std::size_t>(GraphicsCommandLayer::Count);

	/** High-level facade for recording and submitting common graphics operations. */
	class GraphicsFacade
	{
	public:
		// TODO: Decide whether this remains an immediate debug facade or becomes the public 2D/3D drawing API.
		/** @param executor Backend strategy that consumes recorded commands. */
		explicit GraphicsFacade(RenderCommandExecutor& executor) noexcept;

		/** Clears commands from the previous frame while preserving allocated memory. */
		void BeginFrame() noexcept;

		/** @param triangle Triangle coordinates. @param color Fill color. @param sort_key Submission key. */
		void DrawTriangle(ScreenTriangle triangle, ColorRgba color, RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::World);

		/** @param rect Rectangle coordinates. @param color Fill color. @param sort_key Submission key. */
		void DrawRectangle(ScreenRect rect, ColorRgba color, RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::World);

		/** @param rect Rectangle coordinates. @param color Outline color. @param line_width Width in pixels. @param sort_key Submission key. */
		void DrawRectangleOutline(ScreenRect rect, ColorRgba color, float line_width, RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::World);

		/** @param texture Texture handle. @param rect Rectangle coordinates. @param tint Color tint. @param sort_key Submission key. */
		void DrawTexturedQuad(TextureHandle texture, ScreenRect rect, ColorRgba tint, RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::Hud);

		/** @param text Text payload. @param origin Text origin. @param scale Font scale. @param color Text color. @param sort_key Submission key. */
		void DrawText(std::string text, glm::vec2 origin, float scale, ColorRgba color, RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::Hud);

		/** @param center Cube center. @param size Cube size. @param color Fill color. @param sort_key Submission key. */
		void DrawCube(glm::vec3 center, glm::vec3 size, ColorRgba color, RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::World);

		/** @param center Cube center. @param size Cube size. @param color Outline color. @param line_width Width in pixels. @param sort_key Submission key. */
		void DrawCubeOutline(glm::vec3 center, glm::vec3 size, ColorRgba color, float line_width, RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::World);

		/** @param rect Screen-space clipping rectangle. @param sort_key Submission key. */
		void SetScissorRect(ScreenRect rect, RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::Hud);

		/** @param sort_key Submission key. */
		void ClearScissorRect(RenderSortKey sort_key = {}, GraphicsCommandLayer layer = GraphicsCommandLayer::Hud);

		/** Sorts and executes every command recorded for the current frame. */
		void Submit();

		/** @return Number of commands waiting for submission. */
		[[nodiscard]] std::size_t PendingCommandCount() const noexcept;

		/** @param layer Command layer to inspect. @return Commands recorded for that layer. */
		[[nodiscard]] RenderCommandList& Commands(GraphicsCommandLayer layer) noexcept;

		/** @param layer Command layer to inspect. @return Commands recorded for that layer. */
		[[nodiscard]] const RenderCommandList& Commands(GraphicsCommandLayer layer) const noexcept;

	private:
		[[nodiscard]] static constexpr std::size_t LayerIndex(GraphicsCommandLayer layer) noexcept
		{
			return static_cast<std::size_t>(layer);
		}

		RenderCommandExecutor& executor_;
		std::array<RenderCommandList, kGraphicsCommandLayerCount> command_lists_;
	};
}
