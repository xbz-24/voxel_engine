#include <doctest/doctest.h>

#include <voxel/EngineConfig.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace
{
	[[nodiscard]] bool ContainsIssue(const voxel::EngineConfig& config, std::string_view expected)
	{
		const std::vector<std::string> issues = config.Validate();
		return std::ranges::find(issues, expected) != issues.end();
	}
}

TEST_CASE("public camera validation accepts the supported grid coordinate boundaries")
{
	constexpr float upper_exclusive = 0x1p31f;
	const float positive_inside = std::nextafter(upper_exclusive, 0.0f);
	const float negative_inside = std::nextafter(-upper_exclusive, 0.0f);
	const voxel::EngineConfig config = voxel::EngineConfig::Default().WithCamera(
		{ positive_inside, negative_inside, 0.0f },
		{ negative_inside, positive_inside, 0.0f });

	CHECK(config.Validate().empty());
	CHECK(config.IsValid());
}

TEST_CASE("public camera validation rejects values outside the grid coordinate range")
{
	constexpr float boundary = 0x1p31f;
	voxel::EngineConfig config = voxel::EngineConfig::Default().WithCamera(
		{ boundary, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	CHECK(ContainsIssue(config,
		"camera.position must stay within the supported grid coordinate range"));

	config.camera.position = {};
	config.camera.look_at = { -boundary, 0.0f, 0.0f };
	CHECK(ContainsIssue(config,
		"camera.look_at must stay within the supported grid coordinate range"));

	config.camera.look_at.x = std::numeric_limits<float>::infinity();
	CHECK(ContainsIssue(config, "camera.look_at must contain finite values"));
}
