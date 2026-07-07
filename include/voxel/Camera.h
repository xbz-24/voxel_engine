#pragma once

namespace voxel
{
	/** Plain SDK vector type used to avoid exposing a math-library dependency. */
	struct Vec3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	/** Startup camera request passed through EngineConfig. */
	struct CameraConfig
	{
		Vec3 position{ 80.0f, 50.0f, 80.0f };
		Vec3 look_at{ 80.0f, 50.0f, 79.0f };
		bool has_custom_view = false;

		/** Creates a camera override that starts at position and looks toward look_at. */
		[[nodiscard]] static CameraConfig At(Vec3 position, Vec3 look_at) noexcept;
	};

	/** Convenience factory for CameraConfig::At. */
	[[nodiscard]] CameraConfig Camera(Vec3 position, Vec3 look_at) noexcept;
}
