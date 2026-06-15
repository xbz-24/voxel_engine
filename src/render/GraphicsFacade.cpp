#include "GraphicsFacade.h"

#include "RenderCommandSorter.h"

namespace ve::rendering
{
	/** Stores the backend command execution strategy. */
	GraphicsFacade::GraphicsFacade(RenderCommandExecutor& executor) noexcept : executor_(executor)
	{
		command_list_.Reserve(256U);
	}

	/** Starts a new command-recording frame. */
	void GraphicsFacade::BeginFrame() noexcept { command_list_.Clear(); }

	/** Records a filled triangle. */
	void GraphicsFacade::DrawTriangle(ScreenTriangle triangle, ColorRgba color, RenderSortKey sort_key)
	{
		command_list_.DrawTriangle(triangle, color, sort_key);
	}

	/** Records a filled rectangle. */
	void GraphicsFacade::DrawRectangle(ScreenRect rect, ColorRgba color, RenderSortKey sort_key)
	{
		command_list_.DrawSolidRect(rect, color, sort_key);
	}

	/** Records an outlined rectangle. */
	void GraphicsFacade::DrawRectangleOutline(ScreenRect rect, ColorRgba color, float line_width, RenderSortKey sort_key)
	{
		command_list_.DrawRectOutline(rect, color, line_width, sort_key);
	}

	/** Records a filled cube. */
	void GraphicsFacade::DrawCube(glm::vec3 center, glm::vec3 size, ColorRgba color, RenderSortKey sort_key)
	{
		command_list_.DrawSolidCube(center, size, color, sort_key);
	}

	/** Records an outlined cube. */
	void GraphicsFacade::DrawCubeOutline(glm::vec3 center, glm::vec3 size, ColorRgba color, float line_width, RenderSortKey sort_key)
	{
		command_list_.DrawWireCube(center, size, color, line_width, sort_key);
	}

	/** Sorts commands to reduce state changes and executes the selected strategy. */
	void GraphicsFacade::Submit()
	{
		RenderCommandSorter::SortForSubmission(command_list_.MutableCommands());
		executor_.Execute(command_list_);
	}

	/** Returns the number of commands waiting for submission. */
	std::size_t GraphicsFacade::PendingCommandCount() const noexcept { return command_list_.Count(); }
}
