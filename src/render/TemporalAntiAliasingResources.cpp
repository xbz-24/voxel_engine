#include "TemporalAntiAliasingPass.h"

#include "CoreTypes.h"

#include <algorithm>

namespace
{
	ve::core::Index ResourceExtent(int value) noexcept
	{
		return ve::core::ToIndex(std::max(1, value));
	}
}

namespace ve::rendering
{
	GLuint TemporalHistoryResourceCache::PreviousTexture(int frame_index) const noexcept
	{
		return textures[(ve::core::ToIndex(frame_index) + 1U) % textures.size()];
	}

	GLuint TemporalHistoryResourceCache::CurrentTexture(int frame_index) const noexcept
	{
		return textures[ve::core::ToIndex(frame_index) % textures.size()];
	}

	TemporalHistoryResources TemporalAntiAliasingPass::DescribeHistoryResources(int width, int height)
	{
		TemporalHistoryResources resources{};
		resources.previous_history.name = "taa.previous_history";
		resources.previous_history.format = FrameGraphResourceFormat::Rgba16Float;
		resources.previous_history.width = ResourceExtent(width);
		resources.previous_history.height = ResourceExtent(height);
		resources.previous_history.lifetime = FrameGraphResourceLifetime::Imported;
		resources.previous_history.imported = true;

		resources.current_history.name = "taa.current_history";
		resources.current_history.format = FrameGraphResourceFormat::Rgba16Float;
		resources.current_history.width = ResourceExtent(width);
		resources.current_history.height = ResourceExtent(height);
		resources.current_history.lifetime = FrameGraphResourceLifetime::Exported;
		resources.current_history.exported = true;
		return resources;
	}

	const TemporalHistoryResources& TemporalAntiAliasingPass::HistoryResources() const noexcept
	{
		return history_cache_.resources;
	}
}
