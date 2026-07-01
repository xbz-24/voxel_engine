#pragma once

#include "CoreTypes.h"

#include <string>

namespace ve::rendering
{
	enum class FrameGraphResourceFormat
	{
		Unknown,
		Rgba8,
		Rgba16Float,
		Depth24Stencil8
	};

	enum class FrameGraphResourceLifetime
	{
		Transient,
		Imported,
		Exported
	};

	struct FrameGraphResourceDescriptor
	{
		std::string name;
		FrameGraphResourceFormat format = FrameGraphResourceFormat::Unknown;
		ve::core::Index width = 0;
		ve::core::Index height = 0;
		FrameGraphResourceLifetime lifetime = FrameGraphResourceLifetime::Transient;
		bool imported = false;
		bool exported = false;
	};

	struct FrameGraphResourceHandle
	{
		ve::core::Index index = 0;

		[[nodiscard]] friend bool operator==(FrameGraphResourceHandle, FrameGraphResourceHandle) noexcept = default;
	};
}
