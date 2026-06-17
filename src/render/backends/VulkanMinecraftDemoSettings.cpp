#include "VulkanMinecraftDemoSettings.h"

namespace ve::rendering
{
	const char* VulkanMinecraftDemoPresetName(VulkanMinecraftDemoPreset preset) noexcept
	{
		switch (preset)
		{
		case VulkanMinecraftDemoPreset::LakesideVillage: return "Lakeside village";
		case VulkanMinecraftDemoPreset::QuarryOutpost: return "Quarry outpost";
		case VulkanMinecraftDemoPreset::ForestFarm: return "Forest farm";
		case VulkanMinecraftDemoPreset::StressTown: return "Stress town";
		}
		return "Unknown";
	}
}
