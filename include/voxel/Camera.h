#pragma once

namespace voxel
{
	struct Vec3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	struct CameraConfig
	{
		Vec3 position{ 80.0f, 50.0f, 80.0f };
		Vec3 look_at{ 80.0f, 50.0f, 79.0f };
		bool has_custom_view = false;

		[[nodiscard]] static CameraConfig At(Vec3 position, Vec3 look_at) noexcept;
	};

	[[nodiscard]] CameraConfig Camera(Vec3 position, Vec3 look_at) noexcept;
}
