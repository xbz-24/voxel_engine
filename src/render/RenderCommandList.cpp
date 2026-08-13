#include "RenderCommandList.h"

#include <algorithm>

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
