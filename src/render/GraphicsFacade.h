#pragma once

#include "RenderCommandExecutor.h"

namespace ve::rendering
{
	/** High-level facade for recording and submitting common graphics operations. */
	class GraphicsFacade
	{
	public:
		/** @param executor Backend strategy that consumes recorded commands. */
		explicit GraphicsFacade(RenderCommandExecutor& executor) noexcept;

		/** Clears commands from the previous frame while preserving allocated memory. */
		void BeginFrame() noexcept;

		/** @param triangle Triangle coordinates. @param color Fill color. @param sort_key Submission key. */
		void DrawTriangle(ScreenTriangle triangle, ColorRgba color, RenderSortKey sort_key = {});

		/** @param rect Rectangle coordinates. @param color Fill color. @param sort_key Submission key. */
		void DrawRectangle(ScreenRect rect, ColorRgba color, RenderSortKey sort_key = {});

		/** @param rect Rectangle coordinates. @param color Outline color. @param line_width Width in pixels. @param sort_key Submission key. */
		void DrawRectangleOutline(ScreenRect rect, ColorRgba color, float line_width, RenderSortKey sort_key = {});

		/** @param center Cube center. @param size Cube size. @param color Fill color. @param sort_key Submission key. */
		void DrawCube(glm::vec3 center, glm::vec3 size, ColorRgba color, RenderSortKey sort_key = {});

		/** @param center Cube center. @param size Cube size. @param color Outline color. @param line_width Width in pixels. @param sort_key Submission key. */
		void DrawCubeOutline(glm::vec3 center, glm::vec3 size, ColorRgba color, float line_width, RenderSortKey sort_key = {});

		/** Sorts and executes every command recorded for the current frame. */
		void Submit();

		/** @return Number of commands waiting for submission. */
		[[nodiscard]] std::size_t PendingCommandCount() const noexcept;

	private:
		RenderCommandExecutor& executor_;
		RenderCommandList command_list_;
	};
}
