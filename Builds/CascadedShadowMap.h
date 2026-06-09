#pragma once

#include <GL/glew.h>
#include <cstddef>
#include <vector>

namespace ve::rendering
{
	class CascadedShadowMap
	{
	public:
		/** Releases OpenGL shadow resources. */
		~CascadedShadowMap();

		/** @param cascade_count Number of depth cascades. @param resolution Width/height per cascade. @return True when created. */
		bool Initialize(std::size_t cascade_count, int resolution);

		/** @param cascade_index Cascade to render into. */
		void BeginCascade(std::size_t cascade_index) const;

		/** Unbinds the shadow framebuffer. */
		void EndCascade() const;

		/** Releases framebuffer and depth textures. */
		void Release();

		/** @return Number of available cascades. */
		std::size_t CascadeCount() const noexcept;

	private:
		GLuint framebuffer_ = 0;
		std::vector<GLuint> depth_textures_;
		int resolution_ = 0;
	};
}
