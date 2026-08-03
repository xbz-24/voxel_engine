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
