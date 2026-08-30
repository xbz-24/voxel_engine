#include "RenderCommandList.h"

namespace ve::rendering
{
	void RenderCommandList::DrawSolidCube(
		glm::vec3 center, glm::vec3 size, ColorRgba color, RenderSortKey sort_key)
	{
		commands_.push_back(
			RenderCommand{ sort_key, DrawCube3DCommand{ center, size, color, true, 1.0f } });
	}

	void RenderCommandList::DrawWireCube(
		glm::vec3 center, glm::vec3 size, ColorRgba color,
		float line_width, RenderSortKey sort_key)
	{
		commands_.push_back(
			RenderCommand{ sort_key, DrawCube3DCommand{ center, size, color, false, line_width } });
	}

	void RenderCommandList::DrawInstancedMesh(
		std::uint64_t mesh_id, std::uint32_t instance_count, glm::mat4 transform,
		ColorRgba tint, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{
			sort_key, DrawInstancedMeshCommand{ mesh_id, instance_count, transform, tint } });
	}

	void RenderCommandList::SetScissorRect(ScreenRect rect, RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, ScissorRectCommand{ rect, true } });
	}

	void RenderCommandList::ClearScissorRect(RenderSortKey sort_key)
	{
		commands_.push_back(RenderCommand{ sort_key, ScissorRectCommand{ {}, false } });
	}
}
