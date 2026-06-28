#pragma once

namespace ve::rendering
{
	struct VoxelVertex
	{
		// TODO: Pack vertex attributes and add normals/tangents once materials and lighting are backend-driven.
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
		float light = 1.0f;
	};
}
