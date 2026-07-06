#pragma once

#include "FrameGraphResource.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <array>

namespace ve::rendering
{
	struct TemporalHistoryResources
	{
		FrameGraphResourceDescriptor previous_history{};
		FrameGraphResourceDescriptor current_history{};
	};

	struct TemporalHistoryResourceCache
	{
		TemporalHistoryResources resources{};
		GLuint framebuffer = 0;
		std::array<GLuint, 2> textures{{ 0, 0 }};

		[[nodiscard]] GLuint PreviousTexture(int frame_index) const noexcept;
		[[nodiscard]] GLuint CurrentTexture(int frame_index) const noexcept;
	};

	struct TemporalAntiAliasingSettings
	{
		int width = 1;
		int height = 1;
		float history_blend = 0.92f;
		TemporalHistoryResources history_resources{};
	};

	class TemporalAntiAliasingPass
	{
	public:
		~TemporalAntiAliasingPass();

		/** @param settings History target size and blend factor. @return True when complete. */
		[[nodiscard]] bool Initialize(const TemporalAntiAliasingSettings& settings);

		/** @param width Target width in pixels. @param height Target height in pixels. @return History resources for FrameGraph. */
		[[nodiscard]] static TemporalHistoryResources DescribeHistoryResources(int width, int height);

		/** @return Backend-neutral history resource metadata. */
		[[nodiscard]] const TemporalHistoryResources& HistoryResources() const noexcept;

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
		TemporalHistoryResourceCache history_cache_{};
		int width_ = 1;
		int height_ = 1;
		int frame_index_ = 0;
		float history_blend_ = 0.92f;
	};
}
