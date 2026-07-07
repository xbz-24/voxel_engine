#include "VulkanMinecraftDemoSettings.h"

namespace ve::rendering
{
	const char* VulkanMinecraftDemoPresetName(VulkanMinecraftDemoPreset preset) noexcept
	{
		switch (preset)
		{
		case VulkanMinecraftDemoPreset::HyperrealDesert: return "Crystal jungle";
		case VulkanMinecraftDemoPreset::AquaModel: return "Aqua model preview";
		case VulkanMinecraftDemoPreset::SponzaAtrium: return "Sponza atrium preview";
		}
		return "Unknown";
	}
}
