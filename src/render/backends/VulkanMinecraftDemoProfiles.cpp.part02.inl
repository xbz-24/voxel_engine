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
