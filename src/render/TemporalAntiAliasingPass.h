#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace ve::rendering
{
	struct TemporalAntiAliasingSettings
	{
		// TODO: Move history textures behind a backend-neutral temporal resource cache for Vulkan reuse.
		int width = 1;
		int height = 1;
		float history_blend = 0.92f;
	};

	class TemporalAntiAliasingPass
	{
	public:
		~TemporalAntiAliasingPass();

		/** @param settings History target size and blend factor. @return True when complete. */
		[[nodiscard]] bool Initialize(const TemporalAntiAliasingSettings& settings);

		/** Advances the history buffer and jitter sequence by one frame. */
		void AdvanceFrame() noexcept;

		/** Releases history framebuffer and textures. */
		void Release();

		/** @return Current sub-pixel jitter in normalized projection units. */
		[[nodiscard]] glm::vec2 CurrentJitter() const noexcept;

		/** @return Texture containing previous resolved history. */
		[[nodiscard]] GLuint PreviousHistoryTexture() const noexcept;

		/** @return Texture that should receive the current resolve. */
		[[nodiscard]] GLuint CurrentHistoryTexture() const noexcept;

		/** @return Blend factor used by the resolve shader. */
		[[nodiscard]] float HistoryBlend() const noexcept;

	private:
		GLuint framebuffer_ = 0;
		GLuint history_textures_[2]{ 0, 0 };
		int width_ = 1;
		int height_ = 1;
		int frame_index_ = 0;
		float history_blend_ = 0.92f;
	};
}
