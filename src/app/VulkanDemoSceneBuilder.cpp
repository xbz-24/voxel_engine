#include "VulkanDemoSceneBuilder.h"

#include "Block.h"
#include "BlockSolidColor.h"
#include "Chunk.h"
#include "ChunkTerrain.h"
#include "ImportedModel.h"
#include "Logger.h"
#include "ModelAssetLibrary.h"
#include "VulkanMinecraftDemoProfiles.h"
#include "World.h"

#include <stb_image.h>

#include <glm/geometric.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "VulkanDemoSceneCore.inl"
#include "VulkanDemoSceneModelAssets.inl"
#include "VulkanDemoSceneTextureSampling.inl"
#include "VulkanDemoSceneMaterialBlocks.inl"
#include "VulkanDemoSceneModelBlocks.inl"
#include "VulkanDemoSceneVoxelizer.inl"
#include "VulkanDemoSceneTerrainNoise.inl"
#include "VulkanDemoSceneGroundCover.inl"
#include "VulkanDemoSceneCaves.inl"
#include "VulkanDemoSceneRockFields.inl"
#include "VulkanDemoSceneTrees.inl"
#include "VulkanDemoSceneAnimalBuilders.inl"
#include "VulkanDemoScenePlantsAndWildlife.inl"
#include "VulkanDemoSceneDecor.inl"
#include "VulkanDemoSceneModelFallbacks.inl"
#include "VulkanDemoSceneModelDemos.inl"
#include "VulkanDemoSceneBuild.inl"
