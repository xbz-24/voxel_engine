#ifndef VE_VOXEL_MATH_GLSL
#define VE_VOXEL_MATH_GLSL

float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

vec3 saturate(vec3 value)
{
	return clamp(value, vec3(0.0), vec3(1.0));
}

float hash13(vec3 value)
{
	value = fract(value * 0.1031);
	value += dot(value, value.yzx + 33.33);
	return fract((value.x + value.y) * value.z);
}

float screen_dither(vec2 pixel_coordinate)
{
	vec3 hash_source = vec3(pixel_coordinate, pixel_coordinate.x + pixel_coordinate.y);
	return hash13(hash_source) - 0.5;
}

#endif
