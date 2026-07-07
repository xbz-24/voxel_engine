#include "BlockPbrMaterials.h"

namespace ve::blocks
{
	namespace
	{
		/// Builds one PBR material with scalar values.
		ve::rendering::PbrMaterial BuildMaterial(glm::vec3 albedo, float roughness, float metallic = 0.0f)
		{
			return ve::rendering::PbrMaterial({ albedo, metallic, roughness, 1.0f, 0.0f });
		}
	}

	/// Builds default physically based material values for a block.
	ve::rendering::PbrMaterial DefaultPbrMaterialForBlock(BlockId id) noexcept
	{
		using enum BlockId;
		if (id == DiamondOre) return BuildMaterial({ 0.55f, 0.9f, 1.0f }, 0.32f, 0.05f);
		if (id == GoldOre) return BuildMaterial({ 1.0f, 0.75f, 0.22f }, 0.28f, 0.12f);
		if (id == CopperOre) return BuildMaterial({ 0.95f, 0.46f, 0.24f }, 0.38f, 0.1f);
		if (id == IronOre) return BuildMaterial({ 0.8f, 0.67f, 0.55f }, 0.42f, 0.08f);
		if (id == EmeraldOre) return BuildMaterial({ 0.2f, 1.0f, 0.45f }, 0.3f, 0.05f);
		if (id == LapisOre) return BuildMaterial({ 0.2f, 0.32f, 1.0f }, 0.36f, 0.03f);
		if (id == AmethystBlock) return ve::rendering::PbrMaterial({ { 0.72f, 0.45f, 1.0f }, 0.0f, 0.18f, 1.0f, 0.08f });
		if (id == Glass) return BuildMaterial({ 0.85f, 0.95f, 1.0f }, 0.04f);
		if (id == Snow) return BuildMaterial({ 0.95f, 0.97f, 1.0f }, 0.62f);
		if (id == OakLeaves || id == BirchLeaves || id == CherryLeaves) return BuildMaterial({ 0.32f, 0.58f, 0.22f }, 0.9f);
		if (id == OakLog || id == BirchLog || id == SpruceLog || id == CherryLog) return BuildMaterial({ 0.55f, 0.36f, 0.18f }, 0.78f);
		if (id == Obsidian || id == Blackstone || id == Bedrock) return BuildMaterial({ 0.08f, 0.08f, 0.1f }, 0.48f);
		if (id == Sand) return BuildMaterial({ 0.86f, 0.78f, 0.48f }, 0.86f);
		if (id == Sandstone || id == CutSandstone) return BuildMaterial({ 0.78f, 0.66f, 0.42f }, 0.82f);
		if (id == RedSand || id == RedSandstone || id == Terracotta) return BuildMaterial({ 0.68f, 0.38f, 0.22f }, 0.84f);
		if (id == Cactus) return BuildMaterial({ 0.24f, 0.46f, 0.18f }, 0.9f);
		if (id == DeadBush) return BuildMaterial({ 0.45f, 0.29f, 0.12f }, 0.92f);
		return BuildMaterial({ 0.7f, 0.7f, 0.7f }, 0.82f);
	}
}
