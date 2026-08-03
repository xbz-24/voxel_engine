#include "GraphicsFacade.h"

#include "RenderCommandSorter.h"

#include <utility>

namespace ve::rendering
{
	/** Stores the backend command execution strategy. */
	GraphicsFacade::GraphicsFacade(RenderCommandExecutor& executor) noexcept : executor_(executor)
	{
		for (RenderCommandList& command_list : command_lists_)
		{
			command_list.Reserve(128U);
		}
	}

	/** Starts a new command-recording frame. */
	void GraphicsFacade::BeginFrame() noexcept
	{
		for (RenderCommandList& command_list : command_lists_)
		{
			command_list.Clear();
		}
	}

	/** Records a filled triangle. */
	void GraphicsFacade::DrawTriangle(ScreenTriangle triangle, ColorRgba color, RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).DrawTriangle(triangle, color, sort_key);
	}

	/** Records a filled rectangle. */
	void GraphicsFacade::DrawRectangle(ScreenRect rect, ColorRgba color, RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).DrawSolidRect(rect, color, sort_key);
	}

	/** Records an outlined rectangle. */
	void GraphicsFacade::DrawRectangleOutline(
		ScreenRect rect,
		ColorRgba color,
		float line_width,
		RenderSortKey sort_key,
		GraphicsCommandLayer layer)
	{
		Commands(layer).DrawRectOutline(rect, color, line_width, sort_key);
	}

	/** Records a textured quad. */
	void GraphicsFacade::DrawTexturedQuad(
		TextureHandle texture,
		ScreenRect rect,
		ColorRgba tint,
		RenderSortKey sort_key,
		GraphicsCommandLayer layer)
	{
		Commands(layer).DrawTexturedQuad(texture, rect, tint, sort_key);
	}

	/** Records text. */
	void GraphicsFacade::DrawText(
		std::string text,
		glm::vec2 origin,
		float scale,
		ColorRgba color,
		RenderSortKey sort_key,
		GraphicsCommandLayer layer)
	{
		Commands(layer).DrawText(std::move(text), origin, scale, color, sort_key);
	}

	/** Records a filled cube. */
	void GraphicsFacade::DrawCube(glm::vec3 center, glm::vec3 size, ColorRgba color, RenderSortKey sort_key, GraphicsCommandLayer layer)
	{
		Commands(layer).DrawSolidCube(center, size, color, sort_key);
	}

	/** Records an outlined cube. */
	void GraphicsFacade::DrawCubeOutline(
		glm::vec3 center,
		glm::vec3 size,
		ColorRgba color,
		float line_width,
		RenderSortKey sort_key,
		GraphicsCommandLayer layer)
	{
		Commands(layer).DrawWireCube(center, size, color, line_width, sort_key);
