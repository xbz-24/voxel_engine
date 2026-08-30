#include "voxel/Engine.h"

namespace voxel
{
	CameraConfig CameraConfig::At(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig{ position, look_at, true };
	}

	CameraConfig Camera(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig::At(position, look_at);
	}
}
