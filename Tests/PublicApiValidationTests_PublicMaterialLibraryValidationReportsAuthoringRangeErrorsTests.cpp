#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public material library validation reports authoring range errors")
{
	voxel::Material invalid = voxel::Material::Named("invalid");
	invalid.base_color = { -0.1f, 0.5f, 1.2f, 1.0f };
	invalid.metallic = 2.0f;
	invalid.roughness = -0.1f;
	invalid.emission = -1.0f;

	voxel::MaterialLibrary materials{};
	materials.Add(invalid)
		.Add(voxel::Material::Named("duplicate"))
		.Add(voxel::Material::Named("duplicate"))
		.Add(voxel::Material{});

	const std::vector<std::string> issues = materials.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.base_color.r must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.base_color.b must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.metallic must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.roughness must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.emission must be zero or greater") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material name is duplicated: duplicate") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material name must not be empty") != issues.end());
}

