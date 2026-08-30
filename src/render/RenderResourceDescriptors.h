#pragma once

#include "CoreTypes.h"
#include "GraphicsTypes.h"

#include <cstdint>
#include <span>

namespace ve::rendering
{
	enum class RenderTextureFormat
	{
		Unknown,
		Rgba8,
		Depth32Float
	};

	enum class RenderTextureUsage : std::uint32_t
	{
		Sampled = 1u << 0u,
		RenderTarget = 1u << 1u,
		DepthStencil = 1u << 2u,
		TransferDestination = 1u << 3u
	};

	[[nodiscard]] constexpr RenderTextureUsage operator|(
		RenderTextureUsage left, RenderTextureUsage right) noexcept
	{
		return ve::core::NumericCast<RenderTextureUsage>(
			ve::core::ToU32(left) | ve::core::ToU32(right));
	}

	[[nodiscard]] constexpr bool HasUsage(
		RenderTextureUsage value, RenderTextureUsage flag) noexcept
	{
		return (ve::core::ToU32(value) & ve::core::ToU32(flag)) != 0u;
	}

	struct RenderTextureDescriptor
	{
		std::uint32_t width = 1;
		std::uint32_t height = 1;
		std::uint32_t mip_level_count = 1;
		RenderTextureFormat format = RenderTextureFormat::Rgba8;
		RenderTextureUsage usage = RenderTextureUsage::Sampled;
	};

	struct RenderTextureUpload
	{
		RenderTextureDescriptor descriptor{};
		std::span<const std::uint8_t> pixels;
	};

	struct RenderMaterialDescriptor
	{
		TextureHandle base_color_texture{};
		float metallic = 0.0f;
		float roughness = 1.0f;
		float emission = 0.0f;
		bool transparent = false;
	};

	struct RenderTargetDescriptor
	{
		std::uint32_t width = 1;
		std::uint32_t height = 1;
		RenderTextureFormat color_format = RenderTextureFormat::Rgba8;
		RenderTextureFormat depth_format = RenderTextureFormat::Depth32Float;
	};

	struct RenderSceneInstanceDescriptor
	{
		std::uint32_t reserved_entity_capacity = 0;
	};
}
