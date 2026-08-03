	}

	/** Records a scissor enable command. */
	void GraphicsFacade::SetScissorRect(ScreenRect rect, RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).SetScissorRect(rect, sort_key);
	}

	/** Records a scissor disable command. */
	void GraphicsFacade::ClearScissorRect(RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).ClearScissorRect(sort_key);
	}

	/** Sorts commands to reduce state changes and executes the selected strategy. */
	void GraphicsFacade::Submit()
	{
		for (RenderCommandList& command_list : command_lists_)
		{
			if (command_list.Count() == 0U) continue;
			RenderCommandSorter::SortForSubmission(command_list.MutableCommands());
			executor_.Execute(command_list);
		}
	}

	/** Returns the number of commands waiting for submission. */
	std::size_t GraphicsFacade::PendingCommandCount() const noexcept
	{
		std::size_t pending_command_count = 0;
		for (const RenderCommandList& command_list : command_lists_)
		{
			pending_command_count += command_list.Count();
		}
		return pending_command_count;
	}

	RenderCommandList& GraphicsFacade::Commands(GraphicsCommandLayer layer) noexcept
	{
		return command_lists_[LayerIndex(layer)];
	}

	const RenderCommandList& GraphicsFacade::Commands(GraphicsCommandLayer layer) const noexcept
	{
		return command_lists_[LayerIndex(layer)];
	}
}
