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
		static void Build(ve::world::World& world, ve::rendering::VulkanMinecraftDemoSceneConfig config);
	};
}
