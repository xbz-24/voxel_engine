#pragma once

#include <filesystem>

namespace ve::core
{
	[[nodiscard]] std::filesystem::path ExecutableDirectory();
}
