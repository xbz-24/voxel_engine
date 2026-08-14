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
	[[nodiscard]] bool ApplyConfiguration(const LoggerConfiguration& configuration);
	Level MinimumLevel();

	void SetConsoleEnabled(bool isEnabled);
	bool SetFileOutput(const std::filesystem::path& path);
	void SetCallback(std::function<void(std::string)> callback);
	void ClearFileOutput();
	void ResetRuntimeState();

	void Write(Level level, std::string_view message, SourceLocation source = {}) noexcept;
	void Write(Level level, std::string_view category, std::string_view message,
		SourceLocation source = {}) noexcept;
	void Write(Level level, std::string_view message, std::span<const Field> fields,
		SourceLocation source = {}) noexcept;
	void Write(Level level, std::string_view category, std::string_view message,
		std::span<const Field> fields, SourceLocation source = {}) noexcept;

	void Trace(std::string_view message, SourceLocation source = {}) noexcept;
	void Debug(std::string_view message, SourceLocation source = {}) noexcept;
	void Info(std::string_view message, SourceLocation source = {}) noexcept;
	void Warning(std::string_view message, SourceLocation source = {}) noexcept;
	void Error(std::string_view message, SourceLocation source = {}) noexcept;
	void Fatal(std::string_view message, SourceLocation source = {}) noexcept;
}

#include "LogMacros.h"
