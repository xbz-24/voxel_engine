#ifndef VE_VOXEL_NOISE_GLSL
#define VE_VOXEL_NOISE_GLSL

float voxel_hash21(vec2 value)
{
	vec3 source = fract(vec3(value.xyx) * vec3(0.1031, 0.1030, 0.0973));
	source += dot(source, source.yzx + 33.33);
	return fract((source.x + source.y) * source.z);
}

float voxel_value_noise(vec2 position)
{
	vec2 cell = floor(position);
	vec2 local = fract(position);
	vec2 blend = local * local * (3.0 - 2.0 * local);
	float lower = mix(voxel_hash21(cell), voxel_hash21(cell + vec2(1.0, 0.0)), blend.x);
	float upper = mix(voxel_hash21(cell + vec2(0.0, 1.0)), voxel_hash21(cell + vec2(1.0)), blend.x);
	return mix(lower, upper, blend.y);
}

float voxel_fbm(vec2 position)
{
	const mat2 octave_rotation = mat2(0.80, -0.60, 0.60, 0.80);
	float amplitude = 0.52;
	float frequency = 1.0;
	float total = 0.0;
	float normalization = 0.0;
	for (int octave = 0; octave < 5; ++octave)
	{
		total += voxel_value_noise(position * frequency) * amplitude;
		normalization += amplitude;
		position = octave_rotation * position + vec2(11.7, 7.3);
		frequency *= 2.03;
		amplitude *= 0.50;
	}
	return total / max(normalization, 0.001);
}

float voxel_detail_fbm(vec2 position)
{
	const mat2 octave_rotation = mat2(0.80, -0.60, 0.60, 0.80);
	float amplitude = 0.57;
	float frequency = 1.0;
	float total = 0.0;
	float normalization = 0.0;
	for (int octave = 0; octave < 3; ++octave)
	{
		total += voxel_value_noise(position * frequency) * amplitude;
		normalization += amplitude;
		position = octave_rotation * position + vec2(11.7, 7.3);
		frequency *= 2.03;
		amplitude *= 0.50;
	}
	return total / max(normalization, 0.001);
}

#endif
