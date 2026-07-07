#include "TemporalAntiAliasingPass.h"

namespace
{
	/// Computes one Halton sequence value for jitter.
	float Halton(int index, int base) noexcept
	{
		float result = 0.0f;
		float weight = 1.0f / static_cast<float>(base);
		while (index > 0)
		{
			result += weight * static_cast<float>(index % base);
			index /= base;
			weight /= static_cast<float>(base);
		}
		return result;
	}
}

namespace ve::rendering
{
	/// Returns current sub-pixel jitter in normalized projection units.
	glm::vec2 TemporalAntiAliasingPass::CurrentJitter() const noexcept
	{
		const int sequence_index = (frame_index_ % 8) + 1;
		const glm::vec2 jitter(Halton(sequence_index, 2) - 0.5f, Halton(sequence_index, 3) - 0.5f);
		return glm::vec2(jitter.x / static_cast<float>(width_), jitter.y / static_cast<float>(height_));
	}

	/// Returns the texture containing previous resolved history.
	GLuint TemporalAntiAliasingPass::PreviousHistoryTexture() const noexcept
	{
		return history_cache_.PreviousTexture(frame_index_);
	}

	/// Returns the texture that should receive the current resolve.
	GLuint TemporalAntiAliasingPass::CurrentHistoryTexture() const noexcept
	{
		return history_cache_.CurrentTexture(frame_index_);
	}

	/// Returns the blend factor used by the resolve shader.
	float TemporalAntiAliasingPass::HistoryBlend() const noexcept
	{
		return history_blend_;
	}
}
