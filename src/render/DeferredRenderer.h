#pragma once

#include "RenderResources.h"

#include <GL/glew.h>

namespace ve::rendering
{
	using DeferredFramebufferSpecification = RenderTargetDescriptor;

	class DeferredRenderer
	{
	public:
		/** Releases OpenGL framebuffer resources. */
		~DeferredRenderer();

		/** @param specification G-buffer dimensions. @return True when all targets were created. */
		bool Initialize(const DeferredFramebufferSpecification& specification);

		/** Binds the G-buffer for geometry rendering. */
		void BeginGeometryPass() const;

		/** Unbinds the G-buffer after geometry rendering. */
		void EndGeometryPass() const;

		/** Releases all owned framebuffer and texture objects. */
		void Release();

		/** @return True when the G-buffer is ready. */
		bool IsInitialized() const noexcept;

	private:
		GLuint framebuffer_ = 0;
		GLuint albedo_texture_ = 0;
		GLuint normal_texture_ = 0;
		GLuint depth_texture_ = 0;
		int width_ = 0;
		int height_ = 0;
	};
}
