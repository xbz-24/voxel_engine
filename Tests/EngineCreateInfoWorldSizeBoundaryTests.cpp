#include <doctest/doctest.h>

#include "Engine.h"
#include "WorldConfiguration.h"

#include <optional>
#include <string>
#include <vector>

TEST_CASE("engine create info accepts the maximum fixed world size")
{
	ve::engine::EngineCreateInfo create_info{};
	create_info.world_size_chunks = ve::world::MaximumWorldSizeChunks;

	CHECK(ve::engine::ValidateEngineCreateInfo(create_info).empty());
	const std::optional<ve::engine::ValidatedEngineCreateInfo> validated =
		ve::engine::MakeValidatedEngineCreateInfo(create_info);

	REQUIRE(validated.has_value());
	CHECK(validated->Value().world_size_chunks == ve::world::MaximumWorldSizeChunks);
}

TEST_CASE("engine create info rejects an oversized world before application startup")
{
	ve::engine::EngineCreateInfo create_info{};
	create_info.world_size_chunks = ve::world::MaximumWorldSizeChunks + 1;
	const std::vector<std::string> issues =
		ve::engine::ValidateEngineCreateInfo(create_info);

	REQUIRE(issues.size() == 1U);
	CHECK(issues.front() == "world_size_chunks must be no greater than 64");

	std::vector<std::string> builder_issues;
	const std::optional<ve::engine::ValidatedEngineCreateInfo> validated =
		ve::engine::MakeValidatedEngineCreateInfo(create_info, &builder_issues);
	CHECK_FALSE(validated.has_value());
	CHECK(builder_issues == issues);

	EngineApplication engine{ create_info };
	const ve::engine::EngineStartupResult result = engine.StartDetailed();

	CHECK_FALSE(result);
	CHECK(result.failure == ve::engine::EngineStartupFailure::InvalidCreateInfo);
	CHECK(result.message ==
		"EngineCreateInfo is invalid: world_size_chunks must be no greater than 64");
	CHECK_FALSE(engine.IsRunning());
}
