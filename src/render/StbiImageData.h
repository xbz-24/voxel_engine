#pragma once

#include <stb_image.h>

#include <memory>

namespace ve::rendering
{
	struct StbiImageDeleter
	{
		void operator()(stbi_uc* data) const noexcept
		{
			stbi_image_free(data);
		}
	};

	using StbiImageData = std::unique_ptr<stbi_uc, StbiImageDeleter>;
}
