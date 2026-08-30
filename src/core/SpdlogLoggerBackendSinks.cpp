#include "SpdlogLoggerBackend.h"

#include <spdlog/logger.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <filesystem>
#include <vector>

namespace ve::log
{
	namespace
	{
		spdlog::filename_t ToSpdlogFilename(const std::filesystem::path& path)
		{
#ifdef SPDLOG_WCHAR_FILENAMES
			return path.wstring();
#else
			return path.string();
#endif
		}
	}

	bool SpdlogLoggerBackend::RebuildLogger()
	{
		std::vector<spdlog::sink_ptr> sinks;
		if (console_enabled_)
			sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		bool file_output_opened = true;
		if (file_output_path_)
		{
			try
			{
				sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(
					ToSpdlogFilename(*file_output_path_), true));
			}
			catch (const spdlog::spdlog_ex&)
			{
				file_output_path_.reset();
				file_output_opened = false;
			}
			catch (const std::filesystem::filesystem_error&)
			{
				file_output_path_.reset();
				file_output_opened = false;
			}
		}
		if (sinks.empty()) sinks.push_back(std::make_shared<spdlog::sinks::null_sink_mt>());
		logger_ = std::make_shared<spdlog::logger>("voxel_engine", sinks.begin(), sinks.end());
		logger_->set_pattern("%H:%M:%S [%^%l%$] [thread %t] %v");
		SetMinimumLevel(minimum_level_);
		logger_->flush_on(spdlog::level::trace);
		return file_output_opened;
	}
}
