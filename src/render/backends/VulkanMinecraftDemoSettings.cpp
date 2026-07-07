#include "VulkanMinecraftDemoSettings.h"

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] VulkanMinecraftDemoSceneConfig CrystalJungleScene() noexcept
		{
			VulkanMinecraftDemoSceneConfig scene{};
			scene.preset = VulkanMinecraftDemoPreset::HyperrealDesert;
			return scene;
		}

		[[nodiscard]] VulkanMinecraftDemoSceneConfig AquaModelScene() noexcept
		{
			VulkanMinecraftDemoSceneConfig scene{};
			scene.preset = VulkanMinecraftDemoPreset::AquaModel;
			scene.seed = 1701;
			scene.ground_y = 44;
			scene.terrain_radius = 58;
			scene.hill_height = 8;
			scene.tree_count = 0;
			scene.market_stall_count = 0;
			scene.cave = false;
			scene.ruins = false;
			scene.vista_marker_count = 0;
			return scene;
		}

		[[nodiscard]] VulkanMinecraftDemoSceneConfig SponzaAtriumScene() noexcept
		{
			VulkanMinecraftDemoSceneConfig scene{};
			scene.preset = VulkanMinecraftDemoPreset::SponzaAtrium;
			scene.seed = 3209;
			scene.ground_y = 44;
			scene.terrain_radius = 72;
			scene.hill_height = 10;
			scene.tree_count = 0;
			scene.market_stall_count = 0;
			scene.cave = false;
			scene.ruins = false;
			scene.vista_marker_count = 0;
			return scene;
		}
	}

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

	VulkanMinecraftDemoProfile VulkanMinecraftDemoProfileFor(VulkanMinecraftDemoPreset preset) noexcept
	{
		switch (preset)
		{
		case VulkanMinecraftDemoPreset::AquaModel:
			return VulkanMinecraftDemoProfile{
				VulkanMinecraftDemoPreset::AquaModel,
				"Aqua model preview",
				"Voxel Engine - Aqua Model Preview",
				16,
				12,
				{ 120.0f, 82.0f, 168.0f },
				{ 92.0f, 58.0f, 92.0f },
				AquaModelScene(),
				"aqua",
				62,
				128,
				700'000u
			};
		case VulkanMinecraftDemoPreset::SponzaAtrium:
			return VulkanMinecraftDemoProfile{
				VulkanMinecraftDemoPreset::SponzaAtrium,
				"Sponza atrium preview",
				"Voxel Engine - Sponza Atrium Preview",
				20,
				16,
				{ 176.0f, 90.0f, 210.0f },
				{ 128.0f, 58.0f, 116.0f },
				SponzaAtriumScene(),
				"sponza",
				96,
				170,
				850'000u
			};
		case VulkanMinecraftDemoPreset::HyperrealDesert:
		default:
			return VulkanMinecraftDemoProfile{
				VulkanMinecraftDemoPreset::HyperrealDesert,
				"Crystal jungle",
				"Voxel Engine - Crystal Jungle",
				20,
				16,
				{ 154.0f, 82.0f, 292.0f },
				{ 160.0f, 52.0f, 86.0f },
				CrystalJungleScene(),
				{},
				0,
				0,
				0u
			};
		}
	}

	VulkanMinecraftDemoSceneConfig VulkanMinecraftDemoDefaultSceneConfig(VulkanMinecraftDemoPreset preset) noexcept
	{
		return VulkanMinecraftDemoProfileFor(preset).scene;
	}
}
