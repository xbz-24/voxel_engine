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
		// TODO: Move authored desert scene layers into data files consumed by the public SceneGraph/AssetCatalog APIs.
		static void Build(ve::world::World& world, ve::rendering::VulkanMinecraftDemoSceneConfig config);
	};
}
