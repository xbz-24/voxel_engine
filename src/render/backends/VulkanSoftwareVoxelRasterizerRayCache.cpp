#include "VulkanSoftwareVoxelRasterizer.h"

#include "Camera.h"

#include <algorithm>
#include <cmath>

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] bool NearlyEqual(const glm::vec3& left, const glm::vec3& right) noexcept
		{
			constexpr float kEpsilon = 0.0001f;
			return glm::all(glm::lessThan(glm::abs(left - right), glm::vec3{ kEpsilon }));
		}
	}

	void VulkanSoftwareVoxelRasterizer::EnsureRayCache(const Camera& camera, std::uint32_t sample_step)
	{
		const glm::vec3 forward = glm::normalize(camera.GetForward());
		const glm::vec3 right = glm::normalize(camera.GetRight());
		const glm::vec3 up = glm::normalize(camera.GetUp());
		if (ray_cache_valid_ && cached_sample_step_ == sample_step &&
			NearlyEqual(cached_forward_, forward) && NearlyEqual(cached_right_, right) && NearlyEqual(cached_up_, up))
		{
			return;
		}

		const float width = static_cast<float>(render_extent_.width);
		const float height = static_cast<float>(render_extent_.height);
		const float aspect = width / height;
		const float tan_half_fov = std::tan(glm::radians(35.0f));
		ray_cache_.clear();
		ray_cache_.reserve(((render_extent_.width + sample_step - 1u) / sample_step) * ((render_extent_.height + sample_step - 1u) / sample_step));

		for (std::uint32_t y = 0; y < render_extent_.height; y += sample_step)
		{
			for (std::uint32_t x = 0; x < render_extent_.width; x += sample_step)
			{
				const float screen_x = ((static_cast<float>(x) + 0.5f) / width) * 2.0f - 1.0f;
				const float screen_y = 1.0f - ((static_cast<float>(y) + 0.5f) / height) * 2.0f;
				ray_cache_.push_back({ x, y, std::min(x + sample_step, render_extent_.width), std::min(y + sample_step, render_extent_.height),
					glm::normalize(forward + (right * screen_x * tan_half_fov * aspect) + (up * screen_y * tan_half_fov)) });
			}
		}

		cached_forward_ = forward;
		cached_right_ = right;
		cached_up_ = up;
		cached_sample_step_ = sample_step;
		ray_cache_valid_ = true;
	}
}
