#include <doctest/doctest.h>

#include <voxel/EngineConfig.h>

#include <algorithm>
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

TEST_CASE("public config validation rejects unknown world edit values")
{
	voxel::EngineConfig config = voxel::EngineConfig::Default();
	config.world.SetBlock(1, 2, 3, voxel::Stone);
	voxel::WorldEdit invalid_kind{};
	invalid_kind.kind = static_cast<voxel::WorldEdit::Kind>(255);
	invalid_kind.block = voxel::Stone;
	config.world.edits.push_back(invalid_kind);
	voxel::WorldEdit invalid_block{};
	invalid_block.block = static_cast<voxel::Block>(255);
	config.world.edits.push_back(invalid_block);

	const std::vector<std::string> issues = config.Validate();
	CHECK(ContainsIssue(issues, "world.edits contains an unknown edit kind"));
	CHECK(ContainsIssue(issues, "world.edits contains an unknown block"));
	CHECK_FALSE(config.IsValid());
}
