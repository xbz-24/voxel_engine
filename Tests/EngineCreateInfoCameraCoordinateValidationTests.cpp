#include <doctest/doctest.h>

#include "Engine.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>

namespace
{
	[[nodiscard]] bool ContainsIssue(const std::vector<std::string>& issues, std::string_view expected)
	{
		return std::ranges::find(issues, expected) != issues.end();
	}
}

TEST_CASE("engine create info accepts camera coordinates inside the supported grid range")
{
	constexpr float upper_exclusive = 0x1p31f;
	ve::engine::EngineCreateInfo create_info{};
	create_info.has_custom_camera = true;
	create_info.camera_position.x = std::nextafter(upper_exclusive, 0.0f);
	create_info.camera_look_at.x = std::nextafter(-upper_exclusive, 0.0f);

	CHECK(ve::engine::ValidateEngineCreateInfo(create_info).empty());
}

TEST_CASE("engine create info rejects camera coordinates outside the supported grid range")
{
	constexpr float boundary = 0x1p31f;
	ve::engine::EngineCreateInfo create_info{};
	create_info.has_custom_camera = true;
	create_info.camera_position.x = boundary;
	create_info.camera_look_at.x = -boundary;
	const std::vector<std::string> issues = ve::engine::ValidateEngineCreateInfo(create_info);

	CHECK(ContainsIssue(issues,
		"camera_position must stay within the supported grid coordinate range"));
	CHECK(ContainsIssue(issues,
		"camera_look_at must stay within the supported grid coordinate range"));
	CHECK_FALSE(ve::engine::MakeValidatedEngineCreateInfo(create_info).has_value());
}
