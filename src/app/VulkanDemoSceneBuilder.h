#pragma once

#include "VulkanMinecraftDemoSettings.h"

namespace ve::world
{
	class World;
}

namespace ve::engine
{
	class VulkanDemoSceneBuilder
	{
	public:
		// Procedural demo builder; data-driven public SceneGraph loading is tracked in the architecture roadmap.
		static void Build(ve::world::World& world, ve::rendering::VulkanMinecraftDemoSceneConfig config);
	};
}
