#pragma once

#include "RenderCommand.h"

#include <span>
#include <vector>

namespace ve::rendering
{
	/** Command buffer facade for high-level immediate drawing requests. */
	class RenderCommandList
	{
	public:
		/** @param expected_command_count Command capacity to reserve. */
		void Reserve(std::size_t expected_command_count);

		/** Removes every queued draw command. */
		void Clear() noexcept;

		/** @param triangle Triangle coordinates. @param color Fill color. @param sort_key Submission order key. */
		void DrawTriangle(ScreenTriangle triangle, ColorRgba color, RenderSortKey sort_key = {});

		/** @param rect Rectangle coordinates. @param color Fill color. @param sort_key Submission order key. */
		void DrawSolidRect(ScreenRect rect, ColorRgba color, RenderSortKey sort_key = {});

		/** @param rect Rectangle coordinates. @param color Outline color. @param line_width Width in pixels. @param sort_key Submission order key. */
		void DrawRectOutline(ScreenRect rect, ColorRgba color, float line_width, RenderSortKey sort_key = {});

		/** @param center Cube center. @param size Cube size by axis. @param color Fill color. @param sort_key Submission order key. */
		void DrawSolidCube(glm::vec3 center, glm::vec3 size, ColorRgba color, RenderSortKey sort_key = {});

		/** @param center Cube center. @param size Cube size by axis. @param color Outline color. @param line_width Width in pixels. @param sort_key Submission order key. */
		void DrawWireCube(glm::vec3 center, glm::vec3 size, ColorRgba color, float line_width, RenderSortKey sort_key = {});

		/** @return Commands queued for execution. */
		[[nodiscard]] std::span<const RenderCommand> Commands() const noexcept;

		/** @return Mutable commands for sorting and render graph preparation. */
		[[nodiscard]] std::span<RenderCommand> MutableCommands() noexcept;

		/** @return Number of queued commands. */
		[[nodiscard]] std::size_t Count() const noexcept;

	private:
		std::vector<RenderCommand> commands_;
	};
}
