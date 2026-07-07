#pragma once

#include "LogCategory.h"
#include "LogRecord.h"
#include "LoggerConfiguration.h"

#include <filesystem>
#include <functional>
#include <span>
#include <string>
#include <string_view>

namespace ve::log
{
	void SetMinimumLevel(Level level);
	void ApplyConfiguration(const LoggerConfiguration& configuration);
	Level MinimumLevel();

	void SetConsoleEnabled(bool isEnabled);
	bool SetFileOutput(const std::filesystem::path& path);
	void SetCallback(std::function<void(std::string)> callback);
	void ClearFileOutput();

	void Write(Level level, std::string_view message, SourceLocation source = {});
	void Write(Level level, std::string_view category, std::string_view message, SourceLocation source = {});
	void Write(Level level, std::string_view message, std::span<const Field> fields, SourceLocation source = {});
	void Write(Level level, std::string_view category, std::string_view message, std::span<const Field> fields, SourceLocation source = {});

	void Trace(std::string_view message, SourceLocation source = {});
	void Debug(std::string_view message, SourceLocation source = {});
	void Info(std::string_view message, SourceLocation source = {});
	void Warning(std::string_view message, SourceLocation source = {});
	void Error(std::string_view message, SourceLocation source = {});
	void Fatal(std::string_view message, SourceLocation source = {});
}

#include "LogMacros.h"
