#include "ExecutablePath.h"

#include <array>
#include <cstddef>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace ve::core
{
	std::filesystem::path ExecutableDirectory()
	{
#if defined(_WIN32)
		std::array<wchar_t, 32768> executable_path_buffer{};
		const DWORD copied_character_count = GetModuleFileNameW(
			nullptr,
			executable_path_buffer.data(),
			static_cast<DWORD>(executable_path_buffer.size()));
		if (copied_character_count > 0 &&
			static_cast<std::size_t>(copied_character_count) < executable_path_buffer.size())
		{
			return std::filesystem::path{
				std::wstring{ executable_path_buffer.data(), copied_character_count }
			}.parent_path();
		}
#endif
		return std::filesystem::current_path();
	}
}
