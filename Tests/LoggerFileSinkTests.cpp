#include <doctest/doctest.h>

#include "Logger.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

namespace
{
	class TemporaryLogDirectory
	{
	public:
		TemporaryLogDirectory()
			: path(std::filesystem::temp_directory_path() /
				("voxel_logger_" + std::to_string(
					std::chrono::steady_clock::now().time_since_epoch().count())))
		{
			std::filesystem::create_directories(path);
		}
		~TemporaryLogDirectory()
		{
			ve::log::ClearFileOutput();
			std::error_code error;
			std::filesystem::remove_all(path, error);
		}
		std::filesystem::path path;
	};
}

TEST_CASE("logger rejects an unwritable file sink without throwing")
{
	TemporaryLogDirectory temporary;
	const std::filesystem::path blocking_file = temporary.path / "not-a-directory";
	std::ofstream{ blocking_file } << "blocks child creation";
	bool opened = true;

	CHECK_NOTHROW(opened = ve::log::SetFileOutput(blocking_file / "engine.log"));
	CHECK_FALSE(opened);
	CHECK_NOTHROW(ve::log::Info("logger remains usable after file sink failure"));
}

TEST_CASE("logger opens an explicitly configured writable file sink")
{
	TemporaryLogDirectory temporary;
	const std::filesystem::path log_file = temporary.path / "logs/engine.log";

	CHECK(ve::log::SetFileOutput(log_file));
	ve::log::Info("explicit file sink record");
	ve::log::ClearFileOutput();

	CHECK(std::filesystem::is_regular_file(log_file));
}
