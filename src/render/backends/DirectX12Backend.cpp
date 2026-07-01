#include "DirectX12Backend.h"

namespace ve::rendering
{
	/** Returns unavailable capabilities while the DirectX 12 backend is only declared. */
	RenderBackendCapabilities DirectX12Backend::Capabilities() const noexcept
	{
		return RenderBackendCapabilities{
			.compute = RenderFeatureSupport{ .supported = true, .required_major_version = 12 },
			.indirect_draw = RenderFeatureSupport{ .supported = true, .required_major_version = 12 },
			.ray_tracing = RenderFeatureSupport{ .supported = true, .required_major_version = 12, .required_minor_version = 1 },
			.limits = RenderBackendLimits{ .max_texture_dimension_2d = 16384, .max_color_attachments = 8 },
			.is_available = false
		};
	}

	/** Returns the DirectX 12 API identifier. */
	GraphicsApi DirectX12Backend::Api() const noexcept
	{
		return GraphicsApi::DirectX12;
	}
}
