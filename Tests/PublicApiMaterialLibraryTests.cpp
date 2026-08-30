#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <string>
#include <vector>

TEST_CASE("public material library clamps physical values")
{
	voxel::MaterialLibrary materials{};
	materials.Add(voxel::Material::Named("glowing")
		.BaseColor({ -1.0f, 0.8f, 2.0f, 1.0f })
		.Texture("grass")
		.NormalTexture("normal")
		.RoughnessTexture("roughness")
		.MetallicTexture("metallic")
		.OcclusionTexture("occlusion")
		.EmissiveTexture("emissive")
		.Metallic(2.0f)
		.Roughness(-1.0f)
		.Emissive(-2.0f)
		.Transparent());

	const voxel::EngineConfig config = voxel::EngineConfig::Default().WithMaterials(materials);

	REQUIRE(config.materials.materials.size() == 1);
	const voxel::Material& material = config.materials.materials[0];
	CHECK(material.transparent);
	CHECK(material.base_color.r == doctest::Approx(0.0f));
	CHECK(material.base_color.b == doctest::Approx(1.0f));
	CHECK(material.metallic == doctest::Approx(1.0f));
	CHECK(material.roughness == doctest::Approx(0.0f));
	CHECK(material.emission == doctest::Approx(0.0f));
	CHECK(material.normal_texture == "normal");
	CHECK(config.materials.Validate().empty());

	const std::vector<std::string> issues = config.Validate();
	CHECK(std::find(issues.begin(), issues.end(),
		"MaterialLibrary runtime binding is not implemented") != issues.end());
}
