#include "RenderCommandList.h"

#include <algorithm>
#include <utility>

namespace ve::rendering
{
	RenderCommandList::RenderCommandList()
		: command_memory_resource_(inline_command_arena_.data(), inline_command_arena_.size()),
		commands_(&command_memory_resource_)
	{
	}

	RenderCommandList::~RenderCommandList() = default;

	/** Reserves command capacity to avoid repeated allocations during a frame. */
	void RenderCommandList::Reserve(std::size_t expected_command_count)
	{
		reserved_command_count_ = std::max(reserved_command_count_, expected_command_count);
		commands_.reserve(expected_command_count);
	}

	/** Clears queued commands without releasing command storage. */
	void RenderCommandList::Clear()
	{
		commands_.clear();
		ResetFrameArena();
	}

	/** Queues a filled 2D triangle command. */
	void RenderCommandList::DrawTriangle(ScreenTriangle triangle, ColorRgba color, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawTriangle2DCommand{ triangle, color } });
	}

	/** Queues a filled 2D rectangle command. */
	void RenderCommandList::DrawSolidRect(ScreenRect rect, ColorRgba color, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawRect2DCommand{ rect, color, true, 1.0f } });
	}

	/** Queues an outlined 2D rectangle command. */
	void RenderCommandList::DrawRectOutline(ScreenRect rect, ColorRgba color, float line_width, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawRect2DCommand{ rect, color, false, line_width } });
	}

	/** Queues a textured 2D quad command. */
	void RenderCommandList::DrawTexturedQuad(TextureHandle texture, ScreenRect rect, ColorRgba tint, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawTexturedQuad2DCommand{ texture, rect, tint } });
	}

	/** Queues a text command. */
	void RenderCommandList::DrawText(std::string text, glm::vec2 origin, float scale, ColorRgba color, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{
			sort_key,
			DrawText2DCommand{ std::pmr::string{ text, &command_memory_resource_ }, origin, scale, color }
		});
	}

	/** Queues a filled 3D cube command. */
	void RenderCommandList::DrawSolidCube(glm::vec3 center, glm::vec3 size, ColorRgba color, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawCube3DCommand{ center, size, color, true, 1.0f } });
	}

	/** Queues an outlined 3D cube command. */
	void RenderCommandList::DrawWireCube(glm::vec3 center, glm::vec3 size, ColorRgba color, float line_width, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawCube3DCommand{ center, size, color, false, line_width } });
	}

	/** Queues an instanced mesh command. */
	void RenderCommandList::DrawInstancedMesh(
		std::uint64_t mesh_id,
		std::uint32_t instance_count,
		glm::mat4 transform,
		ColorRgba tint,
		RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, DrawInstancedMeshCommand{ mesh_id, instance_count, transform, tint } });
	}

	/** Queues a scissor enable command. */
	void RenderCommandList::SetScissorRect(ScreenRect rect, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, ScissorRectCommand{ rect, true } });
	}

	/** Queues a scissor disable command. */
	void RenderCommandList::ClearScissorRect(RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, ScissorRectCommand{ {}, false } });
	}

	/** Returns read-only queued commands. */
	std::span<const RenderCommand> RenderCommandList::Commands() const noexcept
	{
		return { commands_.data(), commands_.size() };
	}

	/** Returns mutable queued commands. */
	std::span<RenderCommand> RenderCommandList::MutableCommands() noexcept
	{
		return { commands_.data(), commands_.size() };
	}

	/** Returns the queued command count. */
	std::size_t RenderCommandList::Count() const noexcept { return commands_.size(); }

	void RenderCommandList::ResetFrameArena()
	{
		std::pmr::vector<RenderCommand> empty_commands(&command_memory_resource_);
		commands_.swap(empty_commands);
		command_memory_resource_.release();
		if (reserved_command_count_ > 0)
		{
			commands_.reserve(reserved_command_count_);
		}
	}
}
