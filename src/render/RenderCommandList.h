#pragma once

#include "RenderCommand.h"

#include <array>
#include <cstddef>
#include <memory_resource>
#include <span>
#include <string>
#include <vector>

namespace ve::rendering
{
	/** Command buffer facade for high-level immediate drawing requests. */
	class RenderCommandList
	{
	public:
		RenderCommandList();
		~RenderCommandList();

		RenderCommandList(const RenderCommandList&) = delete;
		RenderCommandList& operator=(const RenderCommandList&) = delete;
		RenderCommandList(RenderCommandList&&) = delete;
		RenderCommandList& operator=(RenderCommandList&&) = delete;

		/** @param expected_command_count Command capacity to reserve. */
		void Reserve(std::size_t expected_command_count);

		/** Removes every queued draw command. */
		void Clear();

		/** @param triangle Triangle coordinates. @param color Fill color. @param sort_key Submission order key. */
		void DrawTriangle(ScreenTriangle triangle, ColorRgba color, RenderSortKey sort_key = {});

		/** @param rect Rectangle coordinates. @param color Fill color. @param sort_key Submission order key. */
		void DrawSolidRect(ScreenRect rect, ColorRgba color, RenderSortKey sort_key = {});

		/** @param rect Rectangle coordinates. @param color Outline color. @param line_width Width in pixels. @param sort_key Submission order key. */
		void DrawRectOutline(ScreenRect rect, ColorRgba color, float line_width, RenderSortKey sort_key = {});

		/** @param texture Backend-neutral texture handle. @param rect Rectangle coordinates. @param tint Color tint. @param sort_key Submission order key. */
		void DrawTexturedQuad(TextureHandle texture, ScreenRect rect, ColorRgba tint, RenderSortKey sort_key = {});

		/** @param text UTF-8 text payload. @param origin Text origin in pixels. @param scale Font scale. @param color Text color. @param sort_key Submission order key. */
		void DrawText(std::string text, glm::vec2 origin, float scale, ColorRgba color, RenderSortKey sort_key = {});

		/** @param center Cube center. @param size Cube size by axis. @param color Fill color. @param sort_key Submission order key. */
		void DrawSolidCube(glm::vec3 center, glm::vec3 size, ColorRgba color, RenderSortKey sort_key = {});

		/** @param center Cube center. @param size Cube size by axis. @param color Outline color. @param line_width Width in pixels. @param sort_key Submission order key. */
		void DrawWireCube(glm::vec3 center, glm::vec3 size, ColorRgba color, float line_width, RenderSortKey sort_key = {});

		/** @param mesh_id Backend-owned mesh identifier. @param instance_count Number of instances to draw. @param transform Shared transform. @param tint Instance tint. @param sort_key Submission order key. */
		void DrawInstancedMesh(std::uint64_t mesh_id, std::uint32_t instance_count, glm::mat4 transform, ColorRgba tint, RenderSortKey sort_key = {});

		/** @param rect Screen-space clipping rectangle. @param sort_key Submission order key. */
		void SetScissorRect(ScreenRect rect, RenderSortKey sort_key = {});

		/** @param sort_key Submission order key. */
		void ClearScissorRect(RenderSortKey sort_key = {});

		/** @return Commands queued for execution. */
		[[nodiscard]] std::span<const RenderCommand> Commands() const noexcept;

		/** @return Mutable commands for sorting and render graph preparation. */
		[[nodiscard]] std::span<RenderCommand> MutableCommands() noexcept;

		/** @return Number of queued commands. */
		[[nodiscard]] std::size_t Count() const noexcept;

	private:
		static constexpr std::size_t InlineCommandArenaBytes = 64U * 1024U;

		void ResetFrameArena();

		std::array<std::byte, InlineCommandArenaBytes> inline_command_arena_{};
		std::pmr::monotonic_buffer_resource command_memory_resource_;
		std::pmr::vector<RenderCommand> commands_;
		std::size_t reserved_command_count_ = 0;
	};
}
