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
		switch (id)
		{
		case DiamondOre: return BuildMaterial({ 0.55f, 0.9f, 1.0f }, 0.32f, 0.05f);
		case GoldOre: return BuildMaterial({ 1.0f, 0.75f, 0.22f }, 0.28f, 0.12f);
		case CopperOre: return BuildMaterial({ 0.95f, 0.46f, 0.24f }, 0.38f, 0.1f);
		case IronOre: return BuildMaterial({ 0.8f, 0.67f, 0.55f }, 0.42f, 0.08f);
		case EmeraldOre: return BuildMaterial({ 0.2f, 1.0f, 0.45f }, 0.3f, 0.05f);
		case LapisOre: return BuildMaterial({ 0.2f, 0.32f, 1.0f }, 0.36f, 0.03f);
		case AmethystBlock: return ve::rendering::PbrMaterial({ { 0.72f, 0.45f, 1.0f }, 0.0f, 0.18f, 1.0f, 0.08f });
		case Glass: return BuildMaterial({ 0.85f, 0.95f, 1.0f }, 0.04f);
		case Snow: return BuildMaterial({ 0.95f, 0.97f, 1.0f }, 0.62f);
		case OakLeaves:
		case BirchLeaves: return BuildMaterial({ 0.32f, 0.58f, 0.22f }, 0.9f);
		case OakLog:
		case BirchLog:
		case SpruceLog: return BuildMaterial({ 0.55f, 0.36f, 0.18f }, 0.78f);
		case Obsidian:
		case Blackstone:
		case Bedrock: return BuildMaterial({ 0.08f, 0.08f, 0.1f }, 0.48f);
		case Sand: return BuildMaterial({ 0.86f, 0.78f, 0.48f }, 0.86f);
		default: return BuildMaterial({ 0.7f, 0.7f, 0.7f }, 0.82f);
		}
	}
}
