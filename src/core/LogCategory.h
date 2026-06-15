#pragma once

#include <string_view>

namespace ve::log::category
{
	inline constexpr std::string_view General = "General";
	inline constexpr std::string_view Engine = "Engine";
	inline constexpr std::string_view Assets = "Assets";
	inline constexpr std::string_view Render = "Render";
	inline constexpr std::string_view World = "World";
	inline constexpr std::string_view Network = "Network";
	inline constexpr std::string_view Tasks = "Tasks";
	inline constexpr std::string_view Gameplay = "Gameplay";
}
