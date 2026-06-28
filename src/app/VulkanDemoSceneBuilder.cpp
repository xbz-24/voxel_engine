#include "VulkanDemoSceneBuilder.h"

#include "Block.h"
#include "BlockSolidColor.h"
#include "Chunk.h"
#include "ChunkTerrain.h"
#include "ImportedModel.h"
#include "Logger.h"
#include "ModelAssetLibrary.h"
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

namespace ve::engine
{
	namespace
	{
		using ve::blocks::BlockId;
		using ve::rendering::VulkanMinecraftDemoSceneConfig;

		struct DemoBounds
		{
			int width = 0;
			int depth = 0;
			int height = ve::world::terrain::ChunkHeight;
			int center_x = 0;
			int center_z = 0;
		};

		struct SurfacePoint
		{
			int x = 0;
			int y = 0;
			int z = 0;
		};

		[[nodiscard]] VulkanMinecraftDemoSceneConfig Sanitize(VulkanMinecraftDemoSceneConfig config) noexcept
		{
			config.ground_y = std::clamp(config.ground_y, 34, 76);
			config.terrain_radius = std::clamp(config.terrain_radius, 48, 140);
			config.hill_height = std::clamp(config.hill_height, 4, 22);
			config.house_count = std::clamp(config.house_count, 0, 120);
			config.tree_count = std::clamp(config.tree_count, 0, 520);
			config.farm_rows = std::clamp(config.farm_rows, 0, 48);
			config.water_radius = std::clamp(config.water_radius, 0, 14);
			config.tower_height = std::clamp(config.tower_height, 2, 14);
			config.lantern_count = std::clamp(config.lantern_count, 0, 40);
			config.ore_richness = std::clamp(config.ore_richness, 0, 56);
			config.market_stall_count = std::clamp(config.market_stall_count, 0, 220);
			config.floating_island_count = 0;
			config.ruin_count = std::clamp(config.ruin_count, 0, 8);
			config.bridge_count = 0;
			config.vista_marker_count = std::clamp(config.vista_marker_count, 0, 12);
			config.village = false;
			config.farms = false;
			config.market = false;
			config.floating_islands = false;
			config.beacon = false;
			return config;
		}

		[[nodiscard]] DemoBounds BoundsFor(const ve::world::World& world) noexcept
		{
			const ve::world::WorldMetrics metrics = world.Metrics();
			const int width = metrics.worldSizeChunks * Chunk::CHUNK_WIDTH;
			const int depth = metrics.worldSizeChunks * Chunk::CHUNK_DEPTH;
			return DemoBounds{ width, depth, ve::world::terrain::ChunkHeight, width / 2, depth / 2 };
		}

		[[nodiscard]] bool IsInside(const DemoBounds& bounds, int x, int y, int z) noexcept
		{
			return x >= 0 && x < bounds.width && y >= 0 && y < bounds.height && z >= 0 && z < bounds.depth;
		}

		void SetBlock(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, BlockId block)
		{
			if (IsInside(bounds, x, y, z)) world.SetBlock(x, y, z, block);
		}

		void FillBox(ve::world::World& world, const DemoBounds& bounds, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z, BlockId block)
		{
			const int begin_x = std::max(0, std::min(min_x, max_x));
			const int begin_y = std::max(0, std::min(min_y, max_y));
			const int begin_z = std::max(0, std::min(min_z, max_z));
			const int end_x = std::min(bounds.width - 1, std::max(min_x, max_x));
			const int end_y = std::min(bounds.height - 1, std::max(min_y, max_y));
			const int end_z = std::min(bounds.depth - 1, std::max(min_z, max_z));
			for (int x = begin_x; x <= end_x; ++x)
			{
				for (int y = begin_y; y <= end_y; ++y)
				{
					for (int z = begin_z; z <= end_z; ++z)
					{
						world.SetBlock(x, y, z, block);
					}
				}
			}
		}

		void FillColumnRange(ve::world::World& world, const DemoBounds& bounds, int x, int min_y, int z, int max_y, BlockId block)
		{
			if (max_y < min_y) return;
			FillBox(world, bounds, x, min_y, z, x, max_y, z, block);
		}

		void ResetDemoWorld(ve::world::World& world)
		{
			const ve::world::WorldMetrics metrics = world.Metrics();
			world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ metrics.worldSizeChunks });
		}

		void FillEllipsoid(ve::world::World& world,
			const DemoBounds& bounds,
			int cx,
			int cy,
			int cz,
			int rx,
			int ry,
			int rz,
			BlockId block)
		{
			for (int dx = -rx; dx <= rx; ++dx)
			{
				for (int dy = -ry; dy <= ry; ++dy)
				{
					for (int dz = -rz; dz <= rz; ++dz)
					{
						const float nx = static_cast<float>(dx) / static_cast<float>(std::max(rx, 1));
						const float ny = static_cast<float>(dy) / static_cast<float>(std::max(ry, 1));
						const float nz = static_cast<float>(dz) / static_cast<float>(std::max(rz, 1));
						if ((nx * nx) + (ny * ny) + (nz * nz) <= 1.0f)
						{
							SetBlock(world, bounds, cx + dx, cy + dy, cz + dz, block);
						}
					}
				}
			}
		}

		[[nodiscard]] std::string Lowercase(std::string value)
		{
			std::ranges::transform(value, value.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
			return value;
		}

		[[nodiscard]] bool IsModelExtension(const std::filesystem::path& path)
		{
			const std::string extension = Lowercase(path.extension().string());
			return extension == ".obj" || extension == ".fbx" || extension == ".gltf" || extension == ".glb" ||
				extension == ".dae" || extension == ".stl" || extension == ".ply" || extension == ".3ds" ||
				extension == ".blend" || extension == ".x";
		}

		[[nodiscard]] std::filesystem::path AssetRoot()
		{
#if defined(ROOT_DIR)
			return std::filesystem::path{ ROOT_DIR } / "assets" / "models";
#else
			return std::filesystem::current_path() / "assets" / "models";
#endif
		}

		[[nodiscard]] std::optional<std::filesystem::path> FindModelAsset(std::string_view keyword)
		{
			const std::filesystem::path root = AssetRoot();
			if (!std::filesystem::exists(root)) return std::nullopt;
			const std::string needle = Lowercase(std::string{ keyword });
			for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(root))
			{
				if (!entry.is_regular_file()) continue;
				const std::filesystem::path path = entry.path();
				if (!IsModelExtension(path)) continue;
				const std::string name = Lowercase(path.filename().string());
				const std::string parent = Lowercase(path.parent_path().filename().string());
				if (name.find(needle) != std::string::npos || parent.find(needle) != std::string::npos) return path;
			}
			return std::nullopt;
		}

		[[nodiscard]] std::uint64_t VoxelKey(int x, int y, int z) noexcept
		{
			return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x)) << 40u) ^
				(static_cast<std::uint64_t>(static_cast<std::uint32_t>(y)) << 20u) ^
				static_cast<std::uint64_t>(static_cast<std::uint32_t>(z));
		}

		[[nodiscard]] std::uint32_t StableStringHash(std::string_view value) noexcept
		{
			std::uint32_t hash = 2166136261u;
			for (char ch : value)
			{
				hash ^= static_cast<std::uint8_t>(ch);
				hash *= 16777619u;
			}
			return hash;
		}

		template <std::size_t Count>
		[[nodiscard]] BlockId PickBlock(const std::array<BlockId, Count>& palette, std::uint32_t hash) noexcept
		{
			return palette[hash % Count];
		}

		struct ImportedTexture
		{
			int width = 0;
			int height = 0;
			std::vector<std::uint8_t> pixels;
		};

		using TextureCache = std::unordered_map<std::string, ImportedTexture>;

		[[nodiscard]] bool IsLoaded(const ImportedTexture& texture) noexcept
		{
			return texture.width > 0 && texture.height > 0 && !texture.pixels.empty();
		}

		[[nodiscard]] const ImportedTexture* CachedTexture(TextureCache& cache, const std::filesystem::path& path)
		{
			if (path.empty()) return nullptr;
			const std::string key = path.lexically_normal().string();
			if (const auto existing = cache.find(key); existing != cache.end())
			{
				return IsLoaded(existing->second) ? &existing->second : nullptr;
			}

			ImportedTexture texture;
			if (std::filesystem::exists(path))
			{
				int width = 0;
				int height = 0;
				int channels = 0;
				stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
				if (data != nullptr && width > 0 && height > 0)
				{
					const std::size_t byte_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u;
					texture.width = width;
					texture.height = height;
					texture.pixels.assign(data, data + byte_count);
				}
				stbi_image_free(data);
			}

			const auto inserted = cache.emplace(key, std::move(texture));
			return IsLoaded(inserted.first->second) ? &inserted.first->second : nullptr;
		}

		[[nodiscard]] float NormalizedHeight(const glm::vec3& point, const glm::vec3& minimum, const glm::vec3& maximum) noexcept
		{
			const float height = std::max(maximum.y - minimum.y, 0.0001f);
			return std::clamp((point.y - minimum.y) / height, 0.0f, 1.0f);
		}

		[[nodiscard]] bool HasUsefulMaterialColor(const glm::vec4& color) noexcept
		{
			const float white_distance = std::abs(color.r - 1.0f) + std::abs(color.g - 1.0f) + std::abs(color.b - 1.0f);
			const float black_distance = color.r + color.g + color.b;
			const float saturation = std::max({ color.r, color.g, color.b }) - std::min({ color.r, color.g, color.b });
			return color.a > 0.05f && white_distance > 0.20f && black_distance > 0.08f && saturation > 0.025f;
		}

		[[nodiscard]] bool HasUsefulTextureColor(ve::blocks::SolidBlockColor color) noexcept
		{
			const float white_distance = std::abs(color.r - 1.0f) + std::abs(color.g - 1.0f) + std::abs(color.b - 1.0f);
			const float black_distance = color.r + color.g + color.b;
			return color.a > 0.05f && white_distance > 0.08f && black_distance > 0.06f;
		}

		[[nodiscard]] ve::blocks::SolidBlockColor SampleTextureColor(const ImportedTexture& texture, glm::vec2 uv) noexcept
		{
			uv.x -= std::floor(uv.x);
			uv.y -= std::floor(uv.y);
			const int x = std::clamp(static_cast<int>(uv.x * static_cast<float>(texture.width)), 0, texture.width - 1);
			const int y = std::clamp(static_cast<int>((1.0f - uv.y) * static_cast<float>(texture.height)), 0, texture.height - 1);
			const std::size_t index = ((static_cast<std::size_t>(y) * static_cast<std::size_t>(texture.width)) + static_cast<std::size_t>(x)) * 4u;
			return {
				static_cast<float>(texture.pixels[index]) / 255.0f,
				static_cast<float>(texture.pixels[index + 1u]) / 255.0f,
				static_cast<float>(texture.pixels[index + 2u]) / 255.0f,
				static_cast<float>(texture.pixels[index + 3u]) / 255.0f
			};
		}

		[[nodiscard]] std::optional<ve::blocks::SolidBlockColor> SampleMaterialColor(const ve::assets::ImportedModel& model,
			const ve::assets::ImportedMesh& mesh,
			glm::vec2 uv,
			TextureCache& texture_cache)
		{
			if (mesh.material_index >= model.materials.size()) return std::nullopt;
			const ve::assets::ImportedMaterial& material = model.materials[mesh.material_index];
			if (const ImportedTexture* texture = CachedTexture(texture_cache, material.albedo_texture))
			{
				const ve::blocks::SolidBlockColor sampled = SampleTextureColor(*texture, uv);
				if (HasUsefulTextureColor(sampled)) return sampled;
			}
			if (HasUsefulMaterialColor(material.base_color))
			{
				return ve::blocks::SolidBlockColor{ material.base_color.r, material.base_color.g, material.base_color.b, material.base_color.a };
			}
			return std::nullopt;
		}

		[[nodiscard]] BlockId BlockFromSourceColor(const ve::assets::ImportedModel& model, ve::core::Index material_index)
		{
			if (material_index >= model.materials.size()) return BlockId::Air;
			const glm::vec4 color = model.materials[material_index].base_color;
			return HasUsefulMaterialColor(color) ? ve::blocks::NearestSolidColorBlock({ color.r, color.g, color.b, color.a }) : BlockId::Air;
		}

		[[nodiscard]] BlockId BlockForAquaTriangle(std::string_view hints, float height, std::uint32_t hash) noexcept
		{
			if (hints.find("hair") != std::string::npos || hints.find("blue") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::DiamondOre, BlockId::LapisOre, BlockId::Glass } }, hash);
			if (hints.find("skin") != std::string::npos || hints.find("face") != std::string::npos || hints.find("body") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::Sandstone, BlockId::CutSandstone, BlockId::BirchPlanks } }, hash);
			if (hints.find("cloth") != std::string::npos || hints.find("dress") != std::string::npos || hints.find("skirt") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::Snow, BlockId::Diorite, BlockId::DiamondOre } }, hash);
			if (height > 0.78f) return PickBlock(std::array<BlockId, 3>{ { BlockId::DiamondOre, BlockId::LapisOre, BlockId::Glass } }, hash);
			if (height > 0.58f) return PickBlock(std::array<BlockId, 3>{ { BlockId::Sandstone, BlockId::CutSandstone, BlockId::DiamondOre } }, hash);
			if (height > 0.34f) return PickBlock(std::array<BlockId, 4>{ { BlockId::Snow, BlockId::Diorite, BlockId::DiamondOre, BlockId::AmethystBlock } }, hash);
			return PickBlock(std::array<BlockId, 3>{ { BlockId::Blackstone, BlockId::DiamondOre, BlockId::Snow } }, hash);
		}

		[[nodiscard]] BlockId BlockForSponzaTriangle(std::string_view hints, float height, std::uint32_t hash) noexcept
		{
			if (hints.find("leaf") != std::string::npos || hints.find("plant") != std::string::npos || hints.find("green") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::MossBlock, BlockId::OakLeaves, BlockId::BirchLeaves } }, hash);
			if (hints.find("curtain") != std::string::npos || hints.find("cloth") != std::string::npos || hints.find("red") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::Terracotta, BlockId::RedSandstone, BlockId::Bricks } }, hash);
			if (hints.find("wood") != std::string::npos || hints.find("bark") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::OakLog, BlockId::SpruceLog, BlockId::OakPlanks } }, hash);
			if (hints.find("roof") != std::string::npos || height > 0.82f) return PickBlock(std::array<BlockId, 3>{ { BlockId::Terracotta, BlockId::RedSandstone, BlockId::Blackstone } }, hash);
			return PickBlock(std::array<BlockId, 6>{ { BlockId::Stone, BlockId::Andesite, BlockId::MossyCobblestone, BlockId::Cobblestone, BlockId::Granite, BlockId::Blackstone } }, hash);
		}

		[[nodiscard]] BlockId BlockForImportedTriangle(const ve::assets::ImportedModel& model,
			const ve::assets::ImportedMesh& mesh,
			const glm::vec3& centroid,
			const glm::vec3& minimum,
			const glm::vec3& maximum,
			const std::optional<ve::blocks::SolidBlockColor>& sampled_color)
		{
			std::string hints = Lowercase(mesh.name);
			if (mesh.material_index < model.materials.size())
			{
				const ve::assets::ImportedMaterial& material = model.materials[mesh.material_index];
				hints += " " + Lowercase(material.name + " " + material.albedo_texture.string());
			}
			const std::string source = Lowercase(model.source_path.string());
			const float height = NormalizedHeight(centroid, minimum, maximum);
			const std::uint32_t hash = StableStringHash(hints) + static_cast<std::uint32_t>(height * 997.0f);
			const BlockId sampled_block = sampled_color ? ve::blocks::NearestSolidColorBlock(*sampled_color) : BlockId::Air;
			if (source.find("aqua") != std::string::npos || source.find("konosuba") != std::string::npos)
			{
				return sampled_block != BlockId::Air ? sampled_block : BlockForAquaTriangle(hints, height, hash);
			}
			if (source.find("sponza") != std::string::npos || source.find("atrium") != std::string::npos)
			{
				return sampled_block != BlockId::Air ? sampled_block : BlockForSponzaTriangle(hints, height, hash);
			}
			const BlockId source_color = sampled_block != BlockId::Air ? sampled_block : BlockFromSourceColor(model, mesh.material_index);
			if (source_color != BlockId::Air) return source_color;
			return PickBlock(std::array<BlockId, 4>{ { BlockId::Diorite, BlockId::Stone, BlockId::CutSandstone, BlockId::MossBlock } }, hash);
		}

		[[nodiscard]] bool ModelBounds(const ve::assets::ImportedModel& model, glm::vec3& minimum, glm::vec3& maximum)
		{
			minimum = glm::vec3{ std::numeric_limits<float>::max() };
			maximum = glm::vec3{ std::numeric_limits<float>::lowest() };
			bool has_vertex = false;
			for (const ve::assets::ImportedMesh& mesh : model.meshes)
			{
				for (const ve::assets::ImportedVertex& vertex : mesh.vertices)
				{
					minimum = glm::min(minimum, vertex.position);
					maximum = glm::max(maximum, vertex.position);
					has_vertex = true;
				}
			}
			return has_vertex;
		}

		[[nodiscard]] glm::vec3 ModelToVoxelPosition(const glm::vec3& position,
			const glm::vec3& minimum,
			const glm::vec3& target_origin,
			float scale) noexcept
		{
			return target_origin + ((position - minimum) * scale);
		}

		void PlaceImportedVoxel(ve::world::World& world,
			const DemoBounds& bounds,
			std::unordered_set<std::uint64_t>& occupied,
			const glm::vec3& position,
			BlockId block,
			std::size_t& budget)
		{
			if (budget == 0) return;
			const int x = static_cast<int>(std::round(position.x));
			const int y = static_cast<int>(std::round(position.y));
			const int z = static_cast<int>(std::round(position.z));
			if (!IsInside(bounds, x, y, z)) return;
			const std::uint64_t key = VoxelKey(x, y, z);
			if (!occupied.insert(key).second) return;
			SetBlock(world, bounds, x, y, z, block);
			--budget;
		}

		void VoxelizeTriangle(ve::world::World& world,
			const DemoBounds& bounds,
			std::unordered_set<std::uint64_t>& occupied,
			glm::vec3 a,
			glm::vec3 b,
			glm::vec3 c,
			BlockId block,
			std::size_t& budget)
		{
			const float longest = std::max({ glm::length(a - b), glm::length(b - c), glm::length(c - a) });
			const int samples = std::clamp(static_cast<int>(std::ceil(longest * 1.55f)), 1, 24);
			for (int u = 0; u <= samples && budget > 0; ++u)
			{
				for (int v = 0; v <= samples - u && budget > 0; ++v)
				{
					const float fu = static_cast<float>(u) / static_cast<float>(samples);
					const float fv = static_cast<float>(v) / static_cast<float>(samples);
					const float fw = 1.0f - fu - fv;
					PlaceImportedVoxel(world, bounds, occupied, (a * fu) + (b * fv) + (c * fw), block, budget);
				}
			}
		}

		[[nodiscard]] bool VoxelizeImportedModel(ve::world::World& world,
			const DemoBounds& bounds,
			const ve::assets::ImportedModel& model,
			int base_y,
			int target_extent,
			std::size_t voxel_budget)
		{
			glm::vec3 minimum{};
			glm::vec3 maximum{};
			if (!ModelBounds(model, minimum, maximum)) return false;
			const glm::vec3 source_size = maximum - minimum;
			const float longest_axis = std::max({ source_size.x, source_size.y, source_size.z });
			if (longest_axis <= 0.0001f) return false;
			const float scale = static_cast<float>(target_extent) / longest_axis;
			const glm::vec3 target_size = source_size * scale;
			const glm::vec3 target_origin{
				static_cast<float>(bounds.center_x) - (target_size.x * 0.5f),
				static_cast<float>(base_y),
				static_cast<float>(bounds.center_z) - (target_size.z * 0.5f)
			};

			std::unordered_set<std::uint64_t> occupied;
			occupied.reserve(std::min<std::size_t>(voxel_budget, 300'000u));
			TextureCache texture_cache;
			for (const ve::assets::ImportedMesh& mesh : model.meshes)
			{
				for (std::size_t index = 0; index + 2 < mesh.indices.size() && voxel_budget > 0; index += 3)
				{
					const std::uint32_t ai = mesh.indices[index];
					const std::uint32_t bi = mesh.indices[index + 1u];
					const std::uint32_t ci = mesh.indices[index + 2u];
					if (ai >= mesh.vertices.size() || bi >= mesh.vertices.size() || ci >= mesh.vertices.size()) continue;
					const glm::vec3 source_centroid = (mesh.vertices[ai].position + mesh.vertices[bi].position + mesh.vertices[ci].position) / 3.0f;
					const glm::vec2 source_uv = (mesh.vertices[ai].texture_coordinates + mesh.vertices[bi].texture_coordinates + mesh.vertices[ci].texture_coordinates) / 3.0f;
					const std::optional<ve::blocks::SolidBlockColor> sampled_color = SampleMaterialColor(model, mesh, source_uv, texture_cache);
					const BlockId block = BlockForImportedTriangle(model, mesh, source_centroid, minimum, maximum, sampled_color);
					const glm::vec3 a = ModelToVoxelPosition(mesh.vertices[ai].position, minimum, target_origin, scale);
					const glm::vec3 b = ModelToVoxelPosition(mesh.vertices[bi].position, minimum, target_origin, scale);
					const glm::vec3 c = ModelToVoxelPosition(mesh.vertices[ci].position, minimum, target_origin, scale);
					VoxelizeTriangle(world, bounds, occupied, a, b, c, block, voxel_budget);
				}
			}
			return !occupied.empty();
		}

		[[nodiscard]] float Hash01(int x, int z, int seed) noexcept
		{
			std::uint32_t value = static_cast<std::uint32_t>(x) * 0x9E3779B9u;
			value ^= static_cast<std::uint32_t>(z) * 0x85EBCA6Bu;
			value ^= static_cast<std::uint32_t>(seed) * 0xC2B2AE35u;
			value ^= value >> 16u;
			value *= 0x7FEB352Du;
			value ^= value >> 15u;
			return static_cast<float>(value & 0xFFFFu) / 65535.0f;
		}

		[[nodiscard]] float SignedHash(int x, int z, int seed) noexcept
		{
			return (Hash01(x, z, seed) * 2.0f) - 1.0f;
		}

		[[nodiscard]] int FindGroundY(const ve::world::World& world, const DemoBounds& bounds, int x, int z, int fallback_y)
		{
			if (x < 0 || x >= bounds.width || z < 0 || z >= bounds.depth) return fallback_y;
			for (int y = bounds.height - 2; y >= 1; --y)
			{
				const BlockId block = world.GetBlock(x, y, z);
				if (block != BlockId::Air && block != BlockId::Water && block != BlockId::Glass) return y;
			}
			return fallback_y;
		}

		[[nodiscard]] int TerrainHeightAt(const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config, int x, int z) noexcept
		{
			const float fx = static_cast<float>(x - bounds.center_x);
			const float fz = static_cast<float>(z - bounds.center_z);
			const float distance = std::sqrt((fx * fx) + (fz * fz));
			const float radius = static_cast<float>(std::max(config.terrain_radius, 1));
			const float radial_fade = std::clamp(1.0f - (distance / (radius + 42.0f)), 0.0f, 1.0f);
			const float canopy_a = std::sin((static_cast<float>(x + config.seed) * 0.037f) + (static_cast<float>(z) * 0.019f));
			const float canopy_b = std::sin((static_cast<float>(x - z) * 0.027f) + (static_cast<float>(config.seed) * 0.009f));
			const float ridge = std::cos((static_cast<float>(x) * 0.075f) + (static_cast<float>(z) * 0.041f) -
				(static_cast<float>(config.seed) * 0.007f));
			const float local = SignedHash(x / 3, z / 3, config.seed + 71) * 1.9f;
			const float height =
				static_cast<float>(config.ground_y) +
				(canopy_a * static_cast<float>(config.hill_height) * 0.34f * radial_fade) +
				(canopy_b * static_cast<float>(config.hill_height) * 0.26f) +
				(ridge * 2.8f * radial_fade) +
				local;
			return std::clamp(static_cast<int>(std::round(height)), 6, bounds.height - 10);
		}

		[[nodiscard]] BlockId SurfaceBlockFor(const VulkanMinecraftDemoSceneConfig& config, int x, int z, int surface_y) noexcept
		{
			const float vein_width = 0.014f + (static_cast<float>(config.ore_richness) * 0.0006f);
			const float root_line = std::abs(std::sin((static_cast<float>(x) * 0.24f) + (static_cast<float>(z) * 0.17f) +
				(static_cast<float>(config.seed) * 0.005f)));
			const float warm_patch = Hash01(x / 2, z / 2, config.seed + 19);
			if (root_line < vein_width && warm_patch > 0.30f) return warm_patch > 0.68f ? BlockId::MossyCobblestone : BlockId::MossBlock;
			if (surface_y > config.ground_y + (config.hill_height / 2) && warm_patch < 0.16f) return BlockId::Stone;
			if (warm_patch < 0.16f) return BlockId::MossBlock;
			if (warm_patch > 0.94f) return BlockId::OakLeaves;
			return BlockId::Grass;
		}

		void SetJungleColumn(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config, int x, int z, int surface_y)
		{
			const float layer_noise = Hash01(x / 5, z / 5, config.seed + 43);
			const BlockId base = layer_noise < 0.50f ? BlockId::Stone : (layer_noise < 0.74f ? BlockId::Andesite : BlockId::Deepslate);
			const BlockId stratum = layer_noise < 0.55f ? BlockId::Dirt : (layer_noise < 0.80f ? BlockId::MossyCobblestone : BlockId::Granite);
			const BlockId humus = layer_noise < 0.42f ? BlockId::Dirt : BlockId::MossBlock;
			FillColumnRange(world, bounds, x, 0, z, std::max(0, surface_y - 15), BlockId::Deepslate);
			FillColumnRange(world, bounds, x, std::max(0, surface_y - 13), z, surface_y - 8, base);
			FillColumnRange(world, bounds, x, surface_y - 7, z, surface_y - 4, stratum);
			FillColumnRange(world, bounds, x, surface_y - 3, z, surface_y - 1, humus);
			SetBlock(world, bounds, x, surface_y, z, SurfaceBlockFor(config, x, z, surface_y));
		}

		void BuildJungleTerrain(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int margin = 3;
			for (int x = margin; x < bounds.width - margin; ++x)
			{
				for (int z = margin; z < bounds.depth - margin; ++z)
				{
					const int surface_y = TerrainHeightAt(bounds, config, x, z);
					FillColumnRange(world, bounds, x, surface_y + 1, z, bounds.height - 1, BlockId::Air);
					SetJungleColumn(world, bounds, config, x, z, surface_y);
				}
			}
		}

		void BuildGroundCover(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (config.farm_rows <= 0) return;
			const float threshold = 0.006f + (static_cast<float>(config.farm_rows) * 0.0018f);
			const int radius_sq = config.terrain_radius * config.terrain_radius;
			for (int x = bounds.center_x - config.terrain_radius; x <= bounds.center_x + config.terrain_radius; ++x)
			{
				for (int z = bounds.center_z - config.terrain_radius; z <= bounds.center_z + config.terrain_radius; ++z)
				{
					const int dx = x - bounds.center_x;
					const int dz = z - bounds.center_z;
					if ((dx * dx) + (dz * dz) > radius_sq) continue;
					const float wave = std::sin((static_cast<float>(x) * 0.72f) + (static_cast<float>(z) * 0.22f) + (static_cast<float>(config.seed) * 0.01f));
					if (std::abs(wave) > threshold) continue;
					if (Hash01(x, z, config.seed + 101) < 0.48f) continue;
					const int y = FindGroundY(world, bounds, x, z, config.ground_y);
					const float flower = Hash01(x, z, config.seed + 102);
					const BlockId block = flower < 0.035f ? BlockId::DiamondOre :
						(flower < 0.075f ? BlockId::AmethystBlock : (flower < 0.16f ? BlockId::HayBlock : (flower < 0.55f ? BlockId::MossBlock : BlockId::OakLeaves)));
					SetBlock(world, bounds, x, y + 1, z, block);
				}
			}
		}

		void BuildLagoon(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.water || config.water_radius <= 0) return;
			const int cx = bounds.center_x - 26;
			const int cz = bounds.center_z + 20;
			const int radius = config.water_radius;
			const int water_y = config.ground_y - 3;
			for (int x = cx - radius - 7; x <= cx + radius + 7; ++x)
			{
				for (int z = cz - radius - 7; z <= cz + radius + 7; ++z)
				{
					const int dx = x - cx;
					const int dz = z - cz;
					const int distance_sq = (dx * dx) + (dz * dz);
					if (distance_sq > (radius + 7) * (radius + 7)) continue;
					const bool wet = distance_sq <= radius * radius;
					const bool shore = !wet && distance_sq <= (radius + 3) * (radius + 3);
					const int surface_y = wet ? water_y - 1 : (shore ? water_y : FindGroundY(world, bounds, x, z, config.ground_y));
					FillBox(world, bounds, x, surface_y - 7, z, x, bounds.height - 1, z, BlockId::Air);
					SetJungleColumn(world, bounds, config, x, z, surface_y);
					if (wet)
					{
						SetBlock(world, bounds, x, water_y, z, BlockId::Water);
					}
					else if (shore)
					{
						SetBlock(world, bounds, x, surface_y, z, Hash01(x, z, config.seed + 121) < 0.35f ? BlockId::MossBlock : BlockId::Grass);
					}
				}
			}
		}

		void FillDisc(ve::world::World& world, const DemoBounds& bounds, int cx, int y, int cz, int radius, BlockId block, int noise_seed)
		{
			for (int dx = -radius; dx <= radius; ++dx)
			{
				for (int dz = -radius; dz <= radius; ++dz)
				{
					const float normalized = static_cast<float>((dx * dx) + (dz * dz)) / static_cast<float>(std::max(radius * radius, 1));
					if (normalized > 1.0f) continue;
					if (normalized > 0.78f && Hash01(cx + dx, cz + dz, noise_seed) < 0.35f) continue;
					SetBlock(world, bounds, cx + dx, y, cz + dz, block);
				}
			}
		}

		void BuildMossyOutcrop(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.cave) return;
			const int cx = bounds.center_x + 52;
			const int cz = bounds.center_z - 48;
			const int base_y = FindGroundY(world, bounds, cx, cz, config.ground_y);
			const int levels = std::max(2, config.tower_height / 3);
			for (int level = 0; level < levels; ++level)
			{
				const int radius = std::max(3, 11 - (level * 3));
				const BlockId block = level % 5 == 0 ? BlockId::MossyCobblestone : (level % 3 == 0 ? BlockId::Andesite : BlockId::Stone);
				FillDisc(world, bounds, cx, base_y + 1 + level, cz, radius, block, config.seed + level);
			}
			FillDisc(world, bounds, cx, base_y + levels + 1, cz, 4, BlockId::MossBlock, config.seed + 401);
		}

		void BuildStreamCut(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.cave) return;
			for (int step = -config.terrain_radius; step <= config.terrain_radius; ++step)
			{
				const int cx = bounds.center_x + step;
				const int cz = bounds.center_z - 8 + static_cast<int>(std::round(std::sin(static_cast<float>(step) * 0.12f) * 9.0f));
				const int half_width = 1 + (std::abs(step) % 4);
				for (int offset = -half_width; offset <= half_width; ++offset)
				{
					const int x = cx;
					const int z = cz + offset;
					const int ground = FindGroundY(world, bounds, x, z, config.ground_y);
					const int carve = 1 + (half_width - std::abs(offset)) / 2;
					FillBox(world, bounds, x, ground - carve + 1, z, x, ground + 2, z, BlockId::Air);
					SetBlock(world, bounds, x, ground - carve, z, offset == 0 ? BlockId::Water : BlockId::MossyCobblestone);
					if (std::abs(offset) == half_width) SetBlock(world, bounds, x, ground - carve + 1, z, BlockId::MossBlock);
				}
			}
		}

		void CarveCavePocket(ve::world::World& world, const DemoBounds& bounds, int cx, int cy, int cz, int rx, int ry, int rz)
		{
			for (int dx = -rx; dx <= rx; ++dx)
			{
				for (int dy = -ry; dy <= ry; ++dy)
				{
					for (int dz = -rz; dz <= rz; ++dz)
					{
						const float nx = static_cast<float>(dx) / static_cast<float>(std::max(rx, 1));
						const float ny = static_cast<float>(dy) / static_cast<float>(std::max(ry, 1));
						const float nz = static_cast<float>(dz) / static_cast<float>(std::max(rz, 1));
						const float distance = (nx * nx) + (ny * ny * 1.4f) + (nz * nz);
						if (distance <= 1.0f) SetBlock(world, bounds, cx + dx, cy + dy, cz + dz, BlockId::Air);
						else if (distance <= 1.25f && Hash01(cx + dx, cz + dz, cy + dy) > 0.38f)
						{
							SetBlock(world, bounds, cx + dx, cy + dy, cz + dz, dy < 0 ? BlockId::MossyCobblestone : BlockId::Blackstone);
						}
					}
				}
			}
		}

		void PlaceCaveTorch(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, int side)
		{
			SetBlock(world, bounds, x, y, z, BlockId::SpruceLog);
			SetBlock(world, bounds, x, y + 1, z, BlockId::GoldOre);
			SetBlock(world, bounds, x, y + 2, z, BlockId::SeaLantern);
			SetBlock(world, bounds, x + side, y + 1, z, BlockId::Terracotta);
		}

		void BuildTorchCave(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.cave) return;
			const int start_x = bounds.center_x - 76;
			const int start_z = bounds.center_z + 58;
			for (int step = 0; step < 74; ++step)
			{
				const int x = start_x + step;
				const int z = start_z + static_cast<int>(std::round(std::sin(static_cast<float>(step) * 0.18f) * 7.0f));
				const int ground = FindGroundY(world, bounds, x, z, config.ground_y);
				const int y = std::max(8, ground - 3 - (step / 18));
				CarveCavePocket(world, bounds, x, y + 2, z, 4, 4, 3);
				FillBox(world, bounds, x - 3, y - 1, z - 2, x + 3, y - 1, z + 2, step % 5 == 0 ? BlockId::MossBlock : BlockId::Stone);
				if (step % 11 == 3) PlaceCaveTorch(world, bounds, x, y, z - 3, -1);
				if (step % 17 == 8) PlaceCaveTorch(world, bounds, x, y, z + 3, 1);
			}
		}

		void PlaceBoulder(ve::world::World& world, const DemoBounds& bounds, int cx, int base_y, int cz, int radius, int height, int seed)
		{
			const std::array<BlockId, 5> palette{ BlockId::Granite, BlockId::Andesite, BlockId::MossyCobblestone, BlockId::Stone, BlockId::MossBlock };
			for (int dx = -radius; dx <= radius; ++dx)
			{
				for (int dz = -radius; dz <= radius; ++dz)
				{
					for (int dy = 0; dy <= height; ++dy)
					{
						const float nx = static_cast<float>(dx) / static_cast<float>(std::max(radius, 1));
						const float nz = static_cast<float>(dz) / static_cast<float>(std::max(radius, 1));
						const float ny = static_cast<float>(dy) / static_cast<float>(std::max(height, 1));
						if ((nx * nx) + (nz * nz) + (ny * ny * 1.3f) > 1.05f) continue;
						if (Hash01(cx + dx, cz + dz + dy, seed) < 0.16f) continue;
						const std::size_t palette_index = static_cast<std::size_t>((std::abs(dx) + std::abs(dz) + dy + seed) % static_cast<int>(palette.size()));
						SetBlock(world, bounds, cx + dx, base_y + dy + 1, cz + dz, palette[palette_index]);
					}
				}
			}
		}

		SurfacePoint RandomSurfacePoint(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config, std::mt19937& rng)
		{
			const int min_x = std::clamp(bounds.center_x - config.terrain_radius + 6, 3, std::max(3, bounds.width - 4));
			const int max_x = std::clamp(bounds.center_x + config.terrain_radius - 6, min_x, std::max(min_x, bounds.width - 4));
			const int min_z = std::clamp(bounds.center_z - config.terrain_radius + 6, 3, std::max(3, bounds.depth - 4));
			const int max_z = std::clamp(bounds.center_z + config.terrain_radius - 6, min_z, std::max(min_z, bounds.depth - 4));
			std::uniform_int_distribution<int> x_dist(min_x, max_x);
			std::uniform_int_distribution<int> z_dist(min_z, max_z);
			const int x = x_dist(rng);
			const int z = z_dist(rng);
			return SurfacePoint{ x, FindGroundY(world, bounds, x, z, config.ground_y), z };
		}

		void BuildRockFields(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed + 501));
			for (int index = 0; index < config.house_count; ++index)
			{
				const SurfacePoint point = RandomSurfacePoint(world, bounds, config, rng);
				if (std::abs(point.x - bounds.center_x) < 10 && std::abs(point.z - bounds.center_z) < 10) continue;
				const int radius = 1 + (index % 2);
				const int height = ((index * 3) % 3);
				PlaceBoulder(world, bounds, point.x, point.y, point.z, radius, height, config.seed + index);
			}
		}

		void PlaceUnderstory(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, int seed)
		{
			const float variant = Hash01(x, z, seed);
			const BlockId flower = variant < 0.08f ? BlockId::DiamondOre : (variant < 0.16f ? BlockId::AmethystBlock : BlockId::OakLeaves);
			SetBlock(world, bounds, x, y + 1, z, variant > 0.54f ? BlockId::MossBlock : flower);
			if (variant > 0.30f) SetBlock(world, bounds, x + 1, y + 1, z, BlockId::OakLeaves);
			if (variant > 0.58f) SetBlock(world, bounds, x - 1, y + 1, z, BlockId::BirchLeaves);
			if (variant > 0.78f) SetBlock(world, bounds, x, y + 2, z, BlockId::MossBlock);
		}

		void PlaceCanopyTree(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, int seed)
		{
			const int height = 12 + static_cast<int>(Hash01(x, z, seed) * 12.0f);
			const BlockId trunk = Hash01(x, z, seed + 1) < 0.70f ? BlockId::SpruceLog : BlockId::OakLog;
			const float leaf_roll = Hash01(x, z, seed + 2);
			const BlockId leaves = leaf_roll < 0.58f ? BlockId::OakLeaves : (leaf_roll < 0.90f ? BlockId::BirchLeaves : BlockId::CherryLeaves);
			FillBox(world, bounds, x, y + 1, z, x, y + height, z, trunk);
			FillBox(world, bounds, x - 1, y + 1, z, x - 1, y + 4, z, trunk);
			FillBox(world, bounds, x + 1, y + 1, z, x + 1, y + 4, z, trunk);
			FillBox(world, bounds, x, y + 1, z - 1, x, y + 3, z - 1, trunk);
			FillEllipsoid(world, bounds, x, y + height + 2, z, 6, 4, 6, leaves);
			FillEllipsoid(world, bounds, x - 4, y + height, z + 2, 4, 3, 4, leaves);
			FillEllipsoid(world, bounds, x + 4, y + height - 1, z - 1, 4, 3, 4, leaves);
			if (Hash01(x, z, seed + 5) > 0.52f) FillBox(world, bounds, x - 2, y + height - 6, z + 3, x - 2, y + height, z + 3, BlockId::MossBlock);
			if (Hash01(x, z, seed + 7) > 0.58f) FillBox(world, bounds, x + 3, y + height - 7, z - 2, x + 3, y + height, z - 2, BlockId::MossBlock);
		}

		void PlaceEmergentTree(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, int seed)
		{
			const int height = 24 + static_cast<int>(Hash01(x, z, seed) * 16.0f);
			FillBox(world, bounds, x, y + 1, z, x, y + height, z, BlockId::SpruceLog);
			FillBox(world, bounds, x - 1, y + 1, z, x + 1, y + 6, z, BlockId::SpruceLog);
			FillBox(world, bounds, x, y + 1, z - 1, x, y + 6, z + 1, BlockId::SpruceLog);
			for (int level = 0; level < 5; ++level)
			{
				const int canopy_y = y + height - (level * 4);
				const int radius = std::max(3, 9 - level);
				FillDisc(world, bounds, x, canopy_y, z, radius, level % 2 == 0 ? BlockId::OakLeaves : BlockId::BirchLeaves, seed + level);
				FillDisc(world, bounds, x, canopy_y + 1, z, std::max(2, radius - 2), BlockId::MossBlock, seed + 20 + level);
			}
			if (Hash01(x, z, seed + 8) > 0.42f) FillBox(world, bounds, x - 4, y + height - 18, z + 2, x - 4, y + height - 2, z + 2, BlockId::MossBlock);
			if (Hash01(x, z, seed + 9) > 0.50f) FillBox(world, bounds, x + 5, y + height - 16, z - 3, x + 5, y + height - 1, z - 3, BlockId::MossBlock);
		}

		void PlaceFrog(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 1, y + 1, z, x + 1, y + 1, z + 1, BlockId::MossBlock);
			SetBlock(world, bounds, x - 1, y + 2, z, BlockId::Blackstone);
			SetBlock(world, bounds, x + 1, y + 2, z, BlockId::Blackstone);
			SetBlock(world, bounds, x, y + 2, z + 1, BlockId::BirchLeaves);
		}

		void PlaceBird(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			SetBlock(world, bounds, x, y + 6, z, BlockId::DiamondOre);
			FillBox(world, bounds, x - 2, y + 6, z, x - 1, y + 6, z, BlockId::AmethystBlock);
			FillBox(world, bounds, x + 1, y + 6, z, x + 2, y + 6, z, BlockId::AmethystBlock);
			SetBlock(world, bounds, x, y + 6, z - 1, BlockId::GoldOre);
			SetBlock(world, bounds, x, y + 5, z, BlockId::OakLog);
		}

		void PlaceTapir(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 1, y + 1, z, x + 1, y + 2, z + 2, BlockId::Blackstone);
			FillBox(world, bounds, x - 1, y + 1, z + 3, x + 1, y + 2, z + 3, BlockId::Snow);
			SetBlock(world, bounds, x, y + 3, z + 3, BlockId::Diorite);
		}

		void PlaceCapybara(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 2, y + 1, z, x + 2, y + 2, z + 2, BlockId::OakPlanks);
			FillBox(world, bounds, x - 1, y + 2, z + 3, x + 1, y + 3, z + 3, BlockId::OakLog);
			FillBox(world, bounds, x - 2, y + 1, z, x + 2, y + 1, z, BlockId::Blackstone);
			SetBlock(world, bounds, x - 1, y + 3, z + 4, BlockId::Blackstone);
			SetBlock(world, bounds, x + 1, y + 3, z + 4, BlockId::Blackstone);
		}

		void PlaceDeer(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 1, y + 3, z, x + 1, y + 4, z + 3, BlockId::HayBlock);
			FillBox(world, bounds, x - 1, y + 1, z, x - 1, y + 2, z, BlockId::OakLog);
			FillBox(world, bounds, x + 1, y + 1, z, x + 1, y + 2, z, BlockId::OakLog);
			FillBox(world, bounds, x - 1, y + 1, z + 3, x - 1, y + 2, z + 3, BlockId::OakLog);
			FillBox(world, bounds, x + 1, y + 1, z + 3, x + 1, y + 2, z + 3, BlockId::OakLog);
			SetBlock(world, bounds, x, y + 5, z + 4, BlockId::OakLog);
			SetBlock(world, bounds, x - 1, y + 6, z + 4, BlockId::BirchLog);
			SetBlock(world, bounds, x + 1, y + 6, z + 4, BlockId::BirchLog);
		}

		void ClearAnimalSpace(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 3, y + 1, z - 3, x + 3, y + 8, z + 5, BlockId::Air);
		}

		void BuildJunglePlants(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.trees || config.tree_count <= 0) return;
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed + 601));
			for (int index = 0; index < config.tree_count; ++index)
			{
				const SurfacePoint point = RandomSurfacePoint(world, bounds, config, rng);
				const float tree_roll = Hash01(point.x, point.z, config.seed + index);
				if (tree_roll > 0.86f)
				{
					PlaceEmergentTree(world, bounds, point.x, point.y, point.z, config.seed + index);
				}
				else if (tree_roll > 0.54f)
				{
					PlaceCanopyTree(world, bounds, point.x, point.y, point.z, config.seed + index);
				}
				else
				{
					PlaceUnderstory(world, bounds, point.x, point.y, point.z, config.seed + index);
				}
			}
		}

		void BuildWildlife(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed + 901));
			for (int index = 0; index < 34; ++index)
			{
				const SurfacePoint point = RandomSurfacePoint(world, bounds, config, rng);
				ClearAnimalSpace(world, bounds, point.x, point.y, point.z);
				if (index % 5 == 0) PlaceFrog(world, bounds, point.x, point.y, point.z);
				else if (index % 5 == 1) PlaceBird(world, bounds, point.x, point.y, point.z);
				else if (index % 5 == 2) PlaceTapir(world, bounds, point.x, point.y, point.z);
				else if (index % 5 == 3) PlaceCapybara(world, bounds, point.x, point.y, point.z);
				else PlaceDeer(world, bounds, point.x, point.y, point.z);
			}
		}

		void BuildMossyRuins(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.ruins || config.ruin_count <= 0) return;
			for (int index = 0; index < config.ruin_count; ++index)
			{
				const int x = bounds.center_x - 46 + (index % 4) * 28;
				const int z = bounds.center_z + 52 - (index / 4) * 24;
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				const int height = 1 + (index % 2);
				FillBox(world, bounds, x - 2, y + 1, z - 2, x - 2, y + height, z - 2, BlockId::MossyCobblestone);
				FillBox(world, bounds, x + 2, y + 1, z - 2, x + 2, y + 1, z - 2, BlockId::Stone);
				FillBox(world, bounds, x - 2, y + 1, z + 2, x - 2, y + 1, z + 2, BlockId::Andesite);
				if (index % 3 == 0) FillBox(world, bounds, x - 1, y + 1, z, x + 1, y + 1, z, BlockId::MossBlock);
				SetBlock(world, bounds, x, y + 1, z, BlockId::AmethystBlock);
			}
		}

		void BuildPebbleFields(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int count = config.market_stall_count * (config.stress_blocks ? 16 : 6);
			if (count <= 0) return;
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed + 701));
			const std::array<BlockId, 6> palette{ BlockId::Gravel, BlockId::Granite, BlockId::Andesite, BlockId::Diorite, BlockId::MossBlock, BlockId::OakLeaves };
			for (int index = 0; index < count; ++index)
			{
				const SurfacePoint point = RandomSurfacePoint(world, bounds, config, rng);
				if (Hash01(point.x, point.z, config.seed + index) < 0.25f) continue;
				const std::size_t palette_index = static_cast<std::size_t>(index % static_cast<int>(palette.size()));
				SetBlock(world, bounds, point.x, point.y + 1, point.z, palette[palette_index]);
			}
		}

		void BuildHeatGlints(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.lights || config.lantern_count <= 0) return;
			for (int index = 0; index < config.lantern_count; ++index)
			{
				const float angle = (static_cast<float>(index) / static_cast<float>(std::max(config.lantern_count, 1))) * 6.2831853f;
				const int radius = 18 + ((index * 11) % std::max(config.terrain_radius - 18, 1));
				const int x = bounds.center_x + static_cast<int>(std::round(std::cos(angle) * static_cast<float>(radius)));
				const int z = bounds.center_z + static_cast<int>(std::round(std::sin(angle) * static_cast<float>(radius)));
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				SetBlock(world, bounds, x, y + 1, z, BlockId::Glass);
				if (index % 4 == 0) SetBlock(world, bounds, x, y, z, BlockId::SeaLantern);
			}
		}

		void BuildSurveyMarkers(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (config.vista_marker_count <= 0) return;
			const int radius = std::max(28, config.terrain_radius - 4);
			for (int index = 0; index < config.vista_marker_count; ++index)
			{
				const float angle = (static_cast<float>(index) / static_cast<float>(std::max(config.vista_marker_count, 1))) * 6.2831853f;
				const int x = bounds.center_x + static_cast<int>(std::round(std::cos(angle) * static_cast<float>(radius)));
				const int z = bounds.center_z + static_cast<int>(std::round(std::sin(angle) * static_cast<float>(radius)));
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				SetBlock(world, bounds, x, y + 1, z, BlockId::SeaLantern);
				SetBlock(world, bounds, x, y + 2, z, index % 2 == 0 ? BlockId::DiamondOre : BlockId::AmethystBlock);
			}
		}

		void BuildModelStage(ve::world::World& world, const DemoBounds& bounds, int base_y, int radius)
		{
			FillBox(world, bounds,
				bounds.center_x - radius,
				base_y - 2,
				bounds.center_z - radius,
				bounds.center_x + radius,
				base_y - 2,
				bounds.center_z + radius,
				BlockId::Blackstone);
			FillBox(world, bounds,
				bounds.center_x - radius + 1,
				base_y - 1,
				bounds.center_z - radius + 1,
				bounds.center_x + radius - 1,
				base_y - 1,
				bounds.center_z + radius - 1,
				BlockId::Andesite);
			FillBox(world, bounds,
				bounds.center_x - radius + 2,
				base_y,
				bounds.center_z - 1,
				bounds.center_x + radius - 2,
				base_y,
				bounds.center_z + 1,
				BlockId::Stone);
			FillBox(world, bounds,
				bounds.center_x - 1,
				base_y,
				bounds.center_z - radius + 2,
				bounds.center_x + 1,
				base_y,
				bounds.center_z + radius - 2,
				BlockId::Stone);
		}

		void BuildAquaFallback(ve::world::World& world, const DemoBounds& bounds, int base_y)
		{
			const int x = bounds.center_x;
			const int z = bounds.center_z;
			FillEllipsoid(world, bounds, x, base_y + 30, z, 10, 18, 6, BlockId::Snow);
			FillEllipsoid(world, bounds, x, base_y + 55, z, 8, 10, 7, BlockId::Sandstone);
			FillEllipsoid(world, bounds, x, base_y + 60, z, 12, 9, 10, BlockId::DiamondOre);
			FillEllipsoid(world, bounds, x - 8, base_y + 47, z, 5, 18, 4, BlockId::DiamondOre);
			FillEllipsoid(world, bounds, x + 8, base_y + 47, z, 5, 18, 4, BlockId::DiamondOre);
			FillBox(world, bounds, x - 6, base_y + 12, z - 2, x - 2, base_y + 29, z + 2, BlockId::Sandstone);
			FillBox(world, bounds, x + 2, base_y + 12, z - 2, x + 6, base_y + 29, z + 2, BlockId::Sandstone);
			FillBox(world, bounds, x - 8, base_y, z - 3, x - 3, base_y + 12, z + 3, BlockId::DiamondOre);
			FillBox(world, bounds, x + 3, base_y, z - 3, x + 8, base_y + 12, z + 3, BlockId::DiamondOre);
			FillBox(world, bounds, x - 20, base_y + 36, z, x - 11, base_y + 40, z + 3, BlockId::Sandstone);
			FillBox(world, bounds, x + 11, base_y + 36, z, x + 20, base_y + 40, z + 3, BlockId::Sandstone);
			FillBox(world, bounds, x + 23, base_y + 4, z + 6, x + 25, base_y + 62, z + 8, BlockId::OakLog);
			FillEllipsoid(world, bounds, x + 24, base_y + 66, z + 7, 5, 5, 5, BlockId::SeaLantern);
		}

		void BuildSponzaFallback(ve::world::World& world, const DemoBounds& bounds, int base_y)
		{
			const int cx = bounds.center_x;
			const int cz = bounds.center_z;
			FillBox(world, bounds, cx - 58, base_y, cz - 34, cx + 58, base_y, cz + 34, BlockId::Diorite);
			for (int side = -1; side <= 1; side += 2)
			{
				const int z = cz + (side * 30);
				FillBox(world, bounds, cx - 62, base_y + 1, z - 3, cx + 62, base_y + 4, z + 3, BlockId::Terracotta);
				for (int i = -5; i <= 5; ++i)
				{
					const int x = cx + (i * 11);
					FillBox(world, bounds, x - 2, base_y + 1, z - 2, x + 2, base_y + 34, z + 2, BlockId::Diorite);
					FillEllipsoid(world, bounds, x, base_y + 38, z, 5, 4, 4, BlockId::CutSandstone);
					if (i < 5)
					{
						FillBox(world, bounds, x + 4, base_y + 29, z - 2, x + 7, base_y + 34, z + 2, BlockId::CutSandstone);
					}
				}
			}
			FillBox(world, bounds, cx - 64, base_y + 40, cz - 36, cx + 64, base_y + 44, cz + 36, BlockId::RedSandstone);
			FillBox(world, bounds, cx - 50, base_y + 45, cz - 30, cx + 50, base_y + 48, cz + 30, BlockId::Terracotta);
			for (int x = cx - 44; x <= cx + 44; x += 12)
			{
				FillBox(world, bounds, x - 2, base_y + 1, cz - 6, x + 2, base_y + 9, cz + 6, BlockId::MossBlock);
			}
		}

		void BuildImportedModelDemo(ve::world::World& world,
			const DemoBounds& bounds,
			std::string_view keyword,
			int base_y,
			int stage_radius,
			int target_extent,
			std::size_t voxel_budget,
			void (*fallback)(ve::world::World&, const DemoBounds&, int))
		{
			(void)bounds;
			ResetDemoWorld(world);
			const DemoBounds reset_bounds = BoundsFor(world);
			BuildModelStage(world, reset_bounds, base_y, stage_radius);
			const std::optional<std::filesystem::path> model_path = FindModelAsset(keyword);
			if (!model_path)
			{
				VE_LOG_CATEGORY_WARNING(ve::log::category::World, "Model demo asset not found under assets/models for keyword: " + std::string{ keyword });
				fallback(world, reset_bounds, base_y);
				return;
			}

			ve::assets::ModelAssetLibrary library;
			std::optional<ve::assets::ImportedModel> model = library.ImportModel(*model_path);
			if (!model || !VoxelizeImportedModel(world, reset_bounds, *model, base_y, target_extent, voxel_budget))
			{
				VE_LOG_CATEGORY_WARNING(ve::log::category::World, "Failed to voxelize model demo asset: " + model_path->string());
				fallback(world, reset_bounds, base_y);
				return;
			}
			VE_LOG_CATEGORY_INFO(ve::log::category::World, "Voxelized model demo asset: " + model_path->string());
		}

		void BuildAquaModelDemo(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int base_y = std::max(8, config.ground_y);
			BuildImportedModelDemo(world, bounds, "aqua", base_y, 62, 128, 700'000u, BuildAquaFallback);
		}

		void BuildSponzaAtriumDemo(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int base_y = std::max(8, config.ground_y);
			BuildImportedModelDemo(world, bounds, "sponza", base_y, 96, 170, 850'000u, BuildSponzaFallback);
		}

		class CrystalJungleBuilder
		{
		public:
			CrystalJungleBuilder(ve::world::World& world, VulkanMinecraftDemoSceneConfig config)
				: world_(world),
				  config_(Sanitize(config)),
				  bounds_(BoundsFor(world))
			{
			}

			void Build()
			{
				ResetDemoWorld(world_);
				bounds_ = BoundsFor(world_);
				BuildJungleTerrain(world_, bounds_, config_);
				BuildLagoon(world_, bounds_, config_);
				BuildStreamCut(world_, bounds_, config_);
				BuildTorchCave(world_, bounds_, config_);
				BuildMossyOutcrop(world_, bounds_, config_);
				BuildGroundCover(world_, bounds_, config_);
				BuildRockFields(world_, bounds_, config_);
				BuildPebbleFields(world_, bounds_, config_);
				BuildJunglePlants(world_, bounds_, config_);
				BuildMossyRuins(world_, bounds_, config_);
				BuildWildlife(world_, bounds_, config_);
				BuildHeatGlints(world_, bounds_, config_);
				BuildSurveyMarkers(world_, bounds_, config_);
			}

		private:
			ve::world::World& world_;
			VulkanMinecraftDemoSceneConfig config_;
			DemoBounds bounds_;
		};
	}

	void VulkanDemoSceneBuilder::Build(ve::world::World& world, ve::rendering::VulkanMinecraftDemoSceneConfig config)
	{
		const VulkanMinecraftDemoSceneConfig sanitized = Sanitize(config);
		switch (sanitized.preset)
		{
		case ve::rendering::VulkanMinecraftDemoPreset::AquaModel:
			BuildAquaModelDemo(world, BoundsFor(world), sanitized);
			break;
		case ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium:
			BuildSponzaAtriumDemo(world, BoundsFor(world), sanitized);
			break;
		case ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert:
		default:
		{
			CrystalJungleBuilder builder{ world, sanitized };
			builder.Build();
			break;
		}
		}
	}
}
