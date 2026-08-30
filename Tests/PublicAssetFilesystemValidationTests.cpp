#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace
{
	std::string OverlongWindowsPath()
	{
		return "C:\\" + std::string(40'000, 'x');
	}

	bool ContainsIssueFragment(
		const std::vector<std::string>& issues, std::string_view expected)
	{
		return std::ranges::any_of(issues, [expected](const std::string& issue)
		{
			return issue.find(expected) != std::string::npos;
		});
	}
}

TEST_CASE("asset validation contains search root inspection failures")
{
	const std::string overlong_path = OverlongWindowsPath();
	voxel::AssetCatalog assets{};
	assets.SearchRoot(overlong_path).SearchRoot("");
	std::vector<std::string> issues;

	CHECK_NOTHROW(issues = assets.Validate(true));
	CHECK(ContainsIssueFragment(issues,
		"asset search root could not be inspected"));
	CHECK(ContainsIssueFragment(issues,
		"asset search root must not be empty"));
}

TEST_CASE("asset validation reports overlong file paths without throwing")
{
	const std::string overlong_path = OverlongWindowsPath();
	voxel::AssetCatalog assets{};
	assets.Texture("too-long", overlong_path)
		.Model("empty-path", "")
		.Sound("", voxel::AssetSource::Embedded({ 1U }));
	std::vector<std::string> issues;

	CHECK_NOTHROW(issues = assets.Validate(true));
	CHECK(ContainsIssueFragment(issues,
		"texture asset path does not exist"));
	CHECK(ContainsIssueFragment(issues,
		"model asset path must not be empty"));
	CHECK(ContainsIssueFragment(issues,
		"sound asset name must not be empty"));
}

TEST_CASE("asset validation skips filesystem queries when they are not required")
{
	const std::string overlong_path = OverlongWindowsPath();
	voxel::AssetCatalog assets{};
	assets.SearchRoot(overlong_path).Texture("too-long", overlong_path);

	CHECK(assets.Validate(false).empty());
}
