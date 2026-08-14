#pragma once

#include "WorldLoadComparisonTestSupport.h"

#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>

namespace world_load_test
{
	inline std::filesystem::path NewPath()
	{
		static std::atomic<unsigned> sequence{0};
		const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
		return std::filesystem::temp_directory_path() /
			("voxel_strict_world_" + std::to_string(tick) + "_" +
				std::to_string(sequence.fetch_add(1)) + ".voxelscene");
	}

	class TemporaryWorldPath final
	{
	public:
		TemporaryWorldPath() : _path(NewPath())
		{
			std::error_code ignored;
			std::filesystem::remove(_path, ignored);
		}

		explicit TemporaryWorldPath(std::string_view contents) : TemporaryWorldPath()
		{
			std::ofstream file(_path, std::ios::binary | std::ios::trunc);
			REQUIRE(file);
			file.write(contents.data(), static_cast<std::streamsize>(contents.size()));
			REQUIRE(file.good());
		}

		~TemporaryWorldPath()
		{
			std::error_code ignored;
			std::filesystem::remove(_path, ignored);
		}

		[[nodiscard]] const std::filesystem::path& Get() const noexcept { return _path; }

	private:
		std::filesystem::path _path;
	};

	inline std::string Document(std::string_view records)
	{
		return "voxel-world-config 1\n" + std::string(records);
	}

	inline void CheckRejected(std::string_view contents)
	{
		CAPTURE(contents);
		const TemporaryWorldPath path(contents);
		voxel::WorldConfig output = SentinelWorld();
		CHECK_FALSE(voxel::TryLoadWorldConfig(path.Get().string(), output));
		CHECK(SameWorld(output, SentinelWorld()));
	}

	inline void CheckMissingRejected()
	{
		const TemporaryWorldPath path;
		voxel::WorldConfig output = SentinelWorld();
		CHECK_FALSE(voxel::TryLoadWorldConfig(path.Get().string(), output));
		CHECK(SameWorld(output, SentinelWorld()));
	}
}
