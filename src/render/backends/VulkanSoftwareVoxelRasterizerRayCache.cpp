#include "VulkanSoftwareVoxelRasterizer.h"

#include "Camera.h"

#include <algorithm>
#include <cstddef>
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

		const float render_width = static_cast<float>(render_extent_.width);
		const float render_height = static_cast<float>(render_extent_.height);
		const float aspect = render_width / render_height;
		const float tan_half_fov = std::tan(glm::radians(35.0f));
		ray_cache_.clear();
		const std::uint32_t sample_columns = (render_extent_.width + sample_step - 1u) / sample_step;
		const std::uint32_t sample_rows = (render_extent_.height + sample_step - 1u) / sample_step;
		ray_cache_.reserve(static_cast<std::size_t>(sample_columns) * sample_rows);

		for (std::uint32_t sample_y = 0; sample_y < render_extent_.height; sample_y += sample_step)
		{
			for (std::uint32_t sample_x = 0; sample_x < render_extent_.width; sample_x += sample_step)
			{
				const float screen_x = ((static_cast<float>(sample_x) + 0.5f) / render_width) * 2.0f - 1.0f;
				const float screen_y = 1.0f - ((static_cast<float>(sample_y) + 0.5f) / render_height) * 2.0f;
				const glm::vec3 ray_direction = glm::normalize(
					forward +
					(right * screen_x * tan_half_fov * aspect) +
					(up * screen_y * tan_half_fov));
				ray_cache_.push_back({
					.begin_x = sample_x,
					.begin_y = sample_y,
					.end_x = std::min(sample_x + sample_step, render_extent_.width),
					.end_y = std::min(sample_y + sample_step, render_extent_.height),
					.direction = ray_direction
				});
			}
		}

		cached_forward_ = forward;
		cached_right_ = right;
		cached_up_ = up;
		cached_sample_step_ = sample_step;
		ray_cache_valid_ = true;
	}
}
