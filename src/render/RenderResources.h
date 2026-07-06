#pragma once

#include "GraphicsTypes.h"

#include <cstdint>
#include <memory>
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

	[[nodiscard]] constexpr RenderTextureUsage operator|(RenderTextureUsage left, RenderTextureUsage right) noexcept
	{
		return static_cast<RenderTextureUsage>(
			static_cast<std::uint32_t>(left) | static_cast<std::uint32_t>(right));
	}

	[[nodiscard]] constexpr bool HasUsage(RenderTextureUsage value, RenderTextureUsage flag) noexcept
	{
		return (static_cast<std::uint32_t>(value) & static_cast<std::uint32_t>(flag)) != 0u;
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

	class RenderTexture
	{
	public:
		virtual ~RenderTexture() = default;
		[[nodiscard]] virtual RenderTextureDescriptor Descriptor() const noexcept = 0;
		virtual void Release() = 0;
	};

	class RenderMaterial
	{
	public:
		virtual ~RenderMaterial() = default;
		[[nodiscard]] virtual RenderMaterialDescriptor Descriptor() const noexcept = 0;
		virtual void Release() = 0;
	};

	class RenderTarget
	{
	public:
		virtual ~RenderTarget() = default;
		[[nodiscard]] virtual RenderTargetDescriptor Descriptor() const noexcept = 0;
		virtual void Release() = 0;
	};

	class RenderSceneInstance
	{
	public:
		virtual ~RenderSceneInstance() = default;
		[[nodiscard]] virtual RenderSceneInstanceDescriptor Descriptor() const noexcept = 0;
		virtual void Release() = 0;
	};

	using RenderTexturePtr = std::unique_ptr<RenderTexture>;
	using RenderMaterialPtr = std::unique_ptr<RenderMaterial>;
	using RenderTargetPtr = std::unique_ptr<RenderTarget>;
	using RenderSceneInstancePtr = std::unique_ptr<RenderSceneInstance>;
}
