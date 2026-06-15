#pragma once

#include "RenderApi.h"

#include <concepts>

namespace ve::engine
{
	class OpenGLRenderView;
	class VulkanRenderView;
}

namespace ve::rendering
{
	class GraphicsFacade;
}

namespace ve::engine
{

	/** Backend-neutral view contract consumed by the engine runtime. */
	class RenderView
	{
	public:
		/** Releases resources through the concrete render-view implementation. */
		virtual ~RenderView() = default;

		/** @return Graphics API represented by this view. */
		[[nodiscard]] virtual ve::rendering::GraphicsApi Api() const noexcept = 0;

		/** @return High-level primitive drawing API, or null when the backend has no executor yet. */
		[[nodiscard]] virtual ve::rendering::GraphicsFacade* Graphics() noexcept;

		/** @return Read-only primitive drawing API, or null when unavailable. */
		[[nodiscard]] virtual const ve::rendering::GraphicsFacade* Graphics() const noexcept;

		/** @return OpenGL compatibility adapter, or null when the view is not OpenGL. */
		[[nodiscard]] virtual OpenGLRenderView* AsOpenGLRenderView() noexcept;

		/** @return Read-only OpenGL compatibility adapter, or null when unavailable. */
		[[nodiscard]] virtual const OpenGLRenderView* AsOpenGLRenderView() const noexcept;

		/** @return Vulkan adapter, or null when the view is not Vulkan. */
		[[nodiscard]] virtual VulkanRenderView* AsVulkanRenderView() noexcept;

		/** @return Read-only Vulkan adapter, or null when unavailable. */
		[[nodiscard]] virtual const VulkanRenderView* AsVulkanRenderView() const noexcept;
	};

	template <typename ViewT>
	concept RenderViewAdapter =
		std::same_as<ViewT, OpenGLRenderView> || std::same_as<ViewT, VulkanRenderView>;

	/**
	 * Converts a backend-neutral render view into a specific adapter type.
	 *
	 * @tparam ViewT Concrete adapter type requested by the caller.
	 * @param view Backend-neutral render view to inspect.
	 * @return Adapter pointer when the view matches ViewT; otherwise null.
	 */
	template <RenderViewAdapter ViewT>
	[[nodiscard]] ViewT* TryRenderViewCast(RenderView& view) noexcept
	{
		if constexpr (std::same_as<ViewT, OpenGLRenderView>) return view.AsOpenGLRenderView();
		if constexpr (std::same_as<ViewT, VulkanRenderView>) return view.AsVulkanRenderView();
		return nullptr;
	}

	/**
	 * Converts a read-only backend-neutral render view into a specific adapter type.
	 *
	 * @tparam ViewT Concrete adapter type requested by the caller.
	 * @param view Backend-neutral render view to inspect.
	 * @return Const adapter pointer when the view matches ViewT; otherwise null.
	 */
	template <RenderViewAdapter ViewT>
	[[nodiscard]] const ViewT* TryRenderViewCast(const RenderView& view) noexcept
	{
		if constexpr (std::same_as<ViewT, OpenGLRenderView>) return view.AsOpenGLRenderView();
		if constexpr (std::same_as<ViewT, VulkanRenderView>) return view.AsVulkanRenderView();
		return nullptr;
	}
}
