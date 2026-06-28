#include "voxel/Engine.h"

#include "EngineRuntimeBridge.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <string_view>
#include <utility>

namespace
{
	[[nodiscard]] int ToSerializedBlock(voxel::Block block) noexcept
	{
		return static_cast<int>(static_cast<std::uint8_t>(block));
	}

	[[nodiscard]] voxel::Block FromSerializedBlock(int value) noexcept
	{
		if (value < ToSerializedBlock(voxel::Block::Air) || value > ToSerializedBlock(voxel::Block::DeadBush))
		{
			return voxel::Block::Air;
		}
		return static_cast<voxel::Block>(static_cast<std::uint8_t>(value));
	}
}

namespace voxel
{
	namespace
	{
		[[nodiscard]] float Clamp01(float value) noexcept
		{
			return std::clamp(value, 0.0f, 1.0f);
		}

		[[nodiscard]] Color ClampColor(Color value) noexcept
		{
			return {
				Clamp01(value.r),
				Clamp01(value.g),
				Clamp01(value.b),
				Clamp01(value.a)
			};
		}
	}

	AssetCatalog& AssetCatalog::Texture(std::string name, std::string path)
	{
		textures.push_back(TextureAsset{ std::move(name), std::move(path) });
		return *this;
	}

	AssetCatalog& AssetCatalog::Model(std::string name, std::string path)
	{
		models.push_back(ModelAsset{ std::move(name), std::move(path) });
		return *this;
	}

	AssetCatalog& AssetCatalog::Sound(std::string name, std::string path)
	{
		sounds.push_back(SoundAsset{ std::move(name), std::move(path) });
		return *this;
	}

	CameraConfig CameraConfig::At(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig{ position, look_at, true };
	}

	CameraConfig Camera(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig::At(position, look_at);
	}

	BlockBox BlockBox::Between(BlockPosition first, BlockPosition second) noexcept
	{
		return BlockBox{
			{
				std::min(first.x, second.x),
				std::min(first.y, second.y),
				std::min(first.z, second.z),
			},
			{
				std::max(first.x, second.x),
				std::max(first.y, second.y),
				std::max(first.z, second.z),
			}
		};
	}

	BlockBox Box(BlockPosition first, BlockPosition second) noexcept
	{
		return BlockBox::Between(first, second);
	}

	WorldConfig WorldConfig::SizeChunks(int value)
	{
		WorldConfig config{};
		config.size_chunks = value;
		return config;
	}

	WorldConfig& WorldConfig::WithSizeChunks(int value) noexcept
	{
		size_chunks = value;
		return *this;
	}

	WorldConfig World(int size_chunks)
	{
		return WorldConfig::SizeChunks(size_chunks);
	}

	WorldConfig World()
	{
		return {};
	}

	WorldConfig Scene(int size_chunks)
	{
		return World(size_chunks);
	}

	WorldConfig Scene()
	{
		return World();
	}

	bool SaveWorldConfig(const WorldConfig& world, const std::string& path)
	{
		std::ofstream file(path, std::ios::trunc);
		if (!file) return false;

		file << "voxel-world-config 1\n";
		file << "size " << world.size_chunks << '\n';
		for (const WorldEdit& edit : world.edits)
		{
			if (edit.kind == WorldEdit::Kind::FillBox)
			{
				file << "fill "
					<< edit.box.minimum.x << ' ' << edit.box.minimum.y << ' ' << edit.box.minimum.z << ' '
					<< edit.box.maximum.x << ' ' << edit.box.maximum.y << ' ' << edit.box.maximum.z << ' '
					<< ToSerializedBlock(edit.block) << '\n';
			}
			else
			{
				file << "set "
					<< edit.position.x << ' ' << edit.position.y << ' ' << edit.position.z << ' '
					<< ToSerializedBlock(edit.block) << '\n';
			}
		}
		return static_cast<bool>(file);
	}

	WorldConfig LoadWorldConfig(const std::string& path)
	{
		std::ifstream file(path);
		if (!file) return {};

		std::string header;
		int version = 0;
		file >> header >> version;
		if (header != "voxel-world-config" || version != 1) return {};

		WorldConfig world{};
		std::string command;
		while (file >> command)
		{
			if (command == "size")
			{
				int size_chunks = world.size_chunks;
				file >> size_chunks;
				world.WithSizeChunks(size_chunks);
			}
			else if (command == "set")
			{
				int x = 0;
				int y = 0;
				int z = 0;
				int block = 0;
				file >> x >> y >> z >> block;
				world.SetBlock(x, y, z, FromSerializedBlock(block));
			}
			else if (command == "fill")
			{
				int x1 = 0;
				int y1 = 0;
				int z1 = 0;
				int x2 = 0;
				int y2 = 0;
				int z2 = 0;
				int block = 0;
				file >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> block;
				world.FillBox(x1, y1, z1, x2, y2, z2, FromSerializedBlock(block));
			}
			else
			{
				std::string ignored_line;
				std::getline(file, ignored_line);
			}
		}
		return world;
	}

	WorldConfig& WorldConfig::SetBlock(BlockPosition position, Block block)
	{
		edits.push_back(WorldEdit{
			WorldEdit::Kind::SetBlock,
			position,
			{},
			block
		});
		return *this;
	}

	WorldConfig& WorldConfig::SetBlock(int x, int y, int z, Block block)
	{
		return SetBlock(At(x, y, z), block);
	}

	WorldConfig& WorldConfig::FillBox(BlockBox box, Block block)
	{
		edits.push_back(WorldEdit{
			WorldEdit::Kind::FillBox,
			{},
			box,
			block
		});
		return *this;
	}

	WorldConfig& WorldConfig::FillBox(BlockPosition first, BlockPosition second, Block block)
	{
		return FillBox(Box(first, second), block);
	}

	WorldConfig& WorldConfig::FillBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block)
	{
		return FillBox(At(x1, y1, z1), At(x2, y2, z2), block);
	}

	WorldConfig& WorldConfig::ClearBox(int x1, int y1, int z1, int x2, int y2, int z2)
	{
		return FillBox(x1, y1, z1, x2, y2, z2, Air);
	}

	WorldConfig& WorldConfig::AddFloor(int y, int radius, Block block)
	{
		return AddFloor(0, y, 0, radius, block);
	}

	WorldConfig& WorldConfig::AddFloor(int center_x, int y, int center_z, int radius, Block block)
	{
		return FillBox(
			center_x - radius,
			y,
			center_z - radius,
			center_x + radius,
			y,
			center_z + radius,
			block);
	}

	WorldConfig& WorldConfig::AddPlatform(int y, int radius, Block block)
	{
		return AddFloor(y, radius, block);
	}

	WorldConfig& WorldConfig::AddBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block)
	{
		return FillBox(x1, y1, z1, x2, y2, z2, block);
	}

	WorldConfig& WorldConfig::AddColumn(int x, int y, int z, int height, Block block)
	{
		if (height <= 0)
		{
			return *this;
		}
		return FillBox(x, y, z, x, y + height - 1, z, block);
	}

	WorldConfig& WorldConfig::AddLamp(int x, int y, int z, Block block)
	{
		return SetBlock(x, y, z, block);
	}

	WorldConfig& WorldConfig::AddLightPost(int x, int y, int z, int height, Block post, Block light)
	{
		if (height <= 0)
		{
			return AddLamp(x, y, z, light);
		}
		return AddColumn(x, y, z, height, post)
			.AddLamp(x, y + height, z, light);
	}

	WorldConfig& WorldConfig::AddPathX(int x1, int x2, int y, int z, int half_width, Block block)
	{
		return FillBox(x1, y, z - half_width, x2, y, z + half_width, block);
	}

	WorldConfig& WorldConfig::AddPathZ(int x, int y, int z1, int z2, int half_width, Block block)
	{
		return FillBox(x - half_width, y, z1, x + half_width, y, z2, block);
	}

	WorldConfig& WorldConfig::AddBridgeX(int x1, int x2, int y, int z, int half_width, Block deck, Block rail)
	{
		return FillBox(x1, y, z - half_width, x2, y, z + half_width, deck)
			.FillBox(x1, y + 1, z - half_width - 1, x2, y + 1, z - half_width - 1, rail)
			.FillBox(x1, y + 1, z + half_width + 1, x2, y + 1, z + half_width + 1, rail);
	}

	WorldConfig& WorldConfig::AddBridgeZ(int x, int y, int z1, int z2, int half_width, Block deck, Block rail)
	{
		return FillBox(x - half_width, y, z1, x + half_width, y, z2, deck)
			.FillBox(x - half_width - 1, y + 1, z1, x - half_width - 1, y + 1, z2, rail)
			.FillBox(x + half_width + 1, y + 1, z1, x + half_width + 1, y + 1, z2, rail);
	}

	WorldConfig& WorldConfig::AddHouse(int x, int y, int z, int radius, Block walls, Block roof)
	{
		const int left = x - radius;
		const int right = x + radius;
		const int front = z - radius;
		const int back = z + radius;
		const int wall_top = y + 3;
		const int roof_y = y + 4;

		return AddBox(left, y, front, right, y, back, walls)
			.AddBox(left, y + 1, front, right, wall_top, front, walls)
			.AddBox(left, y + 1, back, right, wall_top, back, walls)
			.AddBox(left, y + 1, front, left, wall_top, back, walls)
			.AddBox(right, y + 1, front, right, wall_top, back, walls)
			.AddBox(left - 1, roof_y, front - 1, right + 1, roof_y, back + 1, roof)
			.ClearBox(x - 1, y + 1, front, x + 1, y + 2, front)
			.ClearBox(x - 1, y + 1, z - 1, x + 1, wall_top, z + 1)
			.AddLamp(x, wall_top, z);
	}

	WorldConfig& WorldConfig::AddPond(int x, int y, int z, int radius, Block water, Block rim)
	{
		return AddFloor(x, y, z, radius, rim)
			.FillBox(x - radius + 1, y, z - radius + 1, x + radius - 1, y, z + radius - 1, water)
			.ClearBox(x - radius + 1, y + 1, z - radius + 1, x + radius - 1, y + 3, z + radius - 1);
	}

	WorldConfig& WorldConfig::AddGarden(int x, int y, int z, int half_width, int half_depth, Block soil, Block crop)
	{
		return FillBox(x - half_width, y, z - half_depth, x + half_width, y, z + half_depth, soil)
			.FillBox(x - half_width, y + 1, z - half_depth, x + half_width, y + 1, z + half_depth, crop)
			.FillBox(x - half_width - 1, y, z - half_depth - 1, x + half_width + 1, y, z - half_depth - 1, OakLog)
			.FillBox(x - half_width - 1, y, z + half_depth + 1, x + half_width + 1, y, z + half_depth + 1, OakLog);
	}

	WorldConfig& WorldConfig::AddTree(int x, int y, int z, Block trunk, Block leaves)
	{
		return AddColumn(x, y, z, 5, trunk)
			.AddBox(x - 2, y + 3, z - 2, x + 2, y + 5, z + 2, leaves)
			.AddBox(x - 1, y + 6, z - 1, x + 1, y + 6, z + 1, leaves);
	}

	Material Material::Named(std::string name)
	{
		Material material{};
		material.name = std::move(name);
		return material;
	}

	Material& Material::BaseColor(Color value) noexcept
	{
		base_color = ClampColor(value);
		return *this;
	}

	Material& Material::Texture(std::string texture_name)
	{
		texture = std::move(texture_name);
		return *this;
	}

	Material& Material::NormalTexture(std::string texture_name)
	{
		normal_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::RoughnessTexture(std::string texture_name)
	{
		roughness_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::MetallicTexture(std::string texture_name)
	{
		metallic_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::OcclusionTexture(std::string texture_name)
	{
		occlusion_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::EmissiveTexture(std::string texture_name)
	{
		emissive_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::Metallic(float value) noexcept
	{
		metallic = Clamp01(value);
		return *this;
	}

	Material& Material::Roughness(float value) noexcept
	{
		roughness = Clamp01(value);
		return *this;
	}

	Material& Material::Emissive(float strength) noexcept
	{
		emission = std::max(0.0f, strength);
		return *this;
	}

	Material& Material::Transparent(bool enabled) noexcept
	{
		transparent = enabled;
		return *this;
	}

	Light Light::Sun(Vec3 direction, float intensity) noexcept
	{
		Light light{};
		light.kind = LightKind::Directional;
		light.direction = direction;
		light.intensity = std::max(0.0f, intensity);
		return light;
	}

	Light Light::Point(Vec3 position, Color color, float intensity, float range) noexcept
	{
		Light light{};
		light.kind = LightKind::Point;
		light.position = position;
		light.color = ClampColor(color);
		light.intensity = std::max(0.0f, intensity);
		light.range = std::max(0.0f, range);
		return light;
	}

	Environment& Environment::Ambient(Color value) noexcept
	{
		ambient = ClampColor(value);
		return *this;
	}

	Environment& Environment::Fog(Color value, float density) noexcept
	{
		fog = ClampColor(value);
		fog_density = std::max(0.0f, density);
		return *this;
	}

	Environment& Environment::TimeOfDay(float hour) noexcept
	{
		time_of_day = std::clamp(hour, 0.0f, 24.0f);
		return *this;
	}

	MaterialLibrary& MaterialLibrary::Add(Material material)
	{
		materials.push_back(std::move(material));
		return *this;
	}

	Transform Transform::At(Vec3 position) noexcept
	{
		Transform transform{};
		transform.position = position;
		return transform;
	}

	Entity Entity::Named(std::string name)
	{
		Entity entity{};
		entity.name = std::move(name);
		return entity;
	}

	Entity& Entity::At(Vec3 position) noexcept
	{
		transform.position = position;
		return *this;
	}

	Entity& Entity::Rotate(Vec3 euler_degrees) noexcept
	{
		transform.rotation = euler_degrees;
		return *this;
	}

	Entity& Entity::Scale(Vec3 value) noexcept
	{
		transform.scale = value;
		return *this;
	}

	Entity& Entity::Model(std::string asset_name)
	{
		model = std::move(asset_name);
		return *this;
	}

	Entity& Entity::Material(std::string material_name)
	{
		material = std::move(material_name);
		return *this;
	}

	Entity& Entity::Visible(bool enabled) noexcept
	{
		visible = enabled;
		return *this;
	}

	SceneGraph& SceneGraph::Add(Entity entity)
	{
		entities.push_back(std::move(entity));
		return *this;
	}

	SceneGraph& SceneGraph::Add(Light light)
	{
		lights.push_back(light);
		return *this;
	}

	SceneGraph& SceneGraph::EnvironmentSettings(Environment value) noexcept
	{
		environment = value;
		return *this;
	}

	bool InputSnapshot::IsDown(Key key) const noexcept
	{
		switch (key)
		{
		case Key::W: return move_forward;
		case Key::A: return move_left;
		case Key::S: return move_back;
		case Key::D: return move_right;
		case Key::Space: return jump;
		case Key::Escape: return escape;
		case Key::F1: return f1;
		case Key::F2: return f2;
		case Key::Unknown:
		default: return false;
		}
	}

	RuntimeCommands& RuntimeCommands::SetBlock(BlockPosition position, Block block)
	{
		world_edits.push_back(WorldEdit{
			WorldEdit::Kind::SetBlock,
			position,
			{},
			block
		});
		return *this;
	}

	RuntimeCommands& RuntimeCommands::SetBlock(int x, int y, int z, Block block)
	{
		return SetBlock(At(x, y, z), block);
	}

	RuntimeCommands& RuntimeCommands::FillBox(BlockBox box, Block block)
	{
		world_edits.push_back(WorldEdit{
			WorldEdit::Kind::FillBox,
			{},
			box,
			block
		});
		return *this;
	}

	RuntimeCommands& RuntimeCommands::FillBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block)
	{
		return FillBox(Box(At(x1, y1, z1), At(x2, y2, z2)), block);
	}

	RuntimeCommands& RuntimeCommands::ClearBox(int x1, int y1, int z1, int x2, int y2, int z2)
	{
		return FillBox(x1, y1, z1, x2, y2, z2, Air);
	}

	RuntimeCommands& RuntimeCommands::RequestClose() noexcept
	{
		request_close = true;
		return *this;
	}

	namespace
	{
		[[nodiscard]] WorldConfig BuildWorldEditDemoScene()
		{
			return Scene()
				.AddPlatform(62, 10, Grass)
				.AddHouse(0, 63, 0, 4, OakPlanks, Bricks)
				.AddGarden(-9, 63, 4)
				.AddPond(9, 63, 4)
				.AddBridgeX(5, 12, 64, 4)
				.AddLightPost(-5, 63, -7)
				.AddLightPost(5, 63, -7)
				.AddTree(-10, 63, -7)
				.AddTree(10, 63, -7, BirchLog, BirchLeaves);
		}

		[[nodiscard]] WorldConfig BuildShowcaseDemoScene()
		{
			return Scene(10)
				.AddPlatform(61, 14, Stone)
				.AddPlatform(62, 13, MossBlock)
				.AddPathZ(0, 63, -14, 14)
				.AddPathX(-14, 14, 63, 0)
				.AddHouse(-8, 63, -8, 3, Bricks, Blackstone)
				.AddHouse(8, 63, -8, 3, CherryPlanks, CherryLeaves)
				.AddPond(-8, 63, 8, 4)
				.AddGarden(8, 63, 7, 3, 4)
				.AddLightPost(-4, 63, -4)
				.AddLightPost(4, 63, -4)
				.AddLightPost(-4, 63, 4)
				.AddLightPost(4, 63, 4)
				.SetBlock(0, 64, 0, DiamondOre)
				.SetBlock(1, 64, 0, EmeraldOre)
				.SetBlock(-1, 64, 0, GoldOre);
		}

		struct ArcadeBoard
		{
			int origin_x = -28;
			int origin_z = -18;
			int base_y = 64;
			int width = 28;
			int height = 18;
			int pixel = 2;
		};

		WorldConfig& AddArcadeArena(WorldConfig& world, const ArcadeBoard& board)
		{
			const int right = board.origin_x + (board.width * board.pixel) - 1;
			const int back = board.origin_z + (board.height * board.pixel) - 1;
			return world
				.FillBox(board.origin_x - 2, board.base_y - 1, board.origin_z - 2, right + 2, board.base_y - 1, back + 2, Blackstone)
				.FillBox(board.origin_x - 1, board.base_y, board.origin_z - 1, right + 1, board.base_y, back + 1, Deepslate)
				.FillBox(board.origin_x, board.base_y + 1, board.origin_z, right, board.base_y + 1, back, Obsidian)
				.FillBox(board.origin_x - 2, board.base_y + 1, board.origin_z - 2, right + 2, board.base_y + 3, board.origin_z - 1, SeaLantern)
				.FillBox(board.origin_x - 2, board.base_y + 1, back + 1, right + 2, board.base_y + 3, back + 2, SeaLantern)
				.FillBox(board.origin_x - 2, board.base_y + 1, board.origin_z - 2, board.origin_x - 1, board.base_y + 3, back + 2, SeaLantern)
				.FillBox(right + 1, board.base_y + 1, board.origin_z - 2, right + 2, board.base_y + 3, back + 2, SeaLantern);
		}

		WorldConfig& AddPixel(WorldConfig& world, const ArcadeBoard& board, int x, int y, Block block, int layer = 0)
		{
			const int world_x = board.origin_x + (x * board.pixel);
			const int world_z = board.origin_z + (y * board.pixel);
			const int world_y = board.base_y + 2 + layer;
			return world.FillBox(world_x, world_y, world_z, world_x + board.pixel - 1, world_y, world_z + board.pixel - 1, block);
		}

		WorldConfig& AddPixelRect(WorldConfig& world, const ArcadeBoard& board, int x, int y, int width, int height, Block block, int layer = 0)
		{
			for (int row = 0; row < height; ++row)
			{
				for (int column = 0; column < width; ++column)
				{
					AddPixel(world, board, x + column, y + row, block, layer);
				}
			}
			return world;
		}

		WorldConfig& AddSprite(WorldConfig& world, const ArcadeBoard& board, int x, int y, std::initializer_list<std::string_view> rows, Block block, int layer = 0)
		{
			int row_index = 0;
			for (const std::string_view row : rows)
			{
				for (int column = 0; column < static_cast<int>(row.size()); ++column)
				{
					if (row[static_cast<std::size_t>(column)] != ' ')
					{
						AddPixel(world, board, x + column, y + row_index, block, layer);
					}
				}
				++row_index;
			}
			return world;
		}

		[[nodiscard]] WorldConfig BuildArcadeSnakeScene()
		{
			WorldConfig world = Scene(8);
			const ArcadeBoard board{};
			AddArcadeArena(world, board);
			AddPixelRect(world, board, 4, 4, 5, 2, MossBlock, 1);
			AddPixelRect(world, board, 8, 6, 2, 4, MossBlock, 1);
			AddPixelRect(world, board, 10, 9, 5, 2, MossBlock, 1);
			AddPixelRect(world, board, 15, 7, 2, 3, MossBlock, 1);
			AddPixelRect(world, board, 17, 6, 3, 2, MossBlock, 1);
			AddPixel(world, board, 21, 5, DiamondOre, 2);
			AddPixel(world, board, 6, 13, EmeraldOre, 2);
			return world.AddLightPost(-34, 64, -24).AddLightPost(34, 64, 24);
		}

		[[nodiscard]] WorldConfig BuildArcadePaddleScene()
		{
			WorldConfig world = Scene(8);
			const ArcadeBoard board{};
			AddArcadeArena(world, board);
			for (int y = 1; y < board.height - 1; y += 2)
			{
				AddPixel(world, board, board.width / 2, y, Glass, 1);
			}
			AddPixelRect(world, board, 3, 6, 1, 6, OakPlanks, 2);
			AddPixelRect(world, board, 24, 4, 1, 6, CherryPlanks, 2);
			AddPixelRect(world, board, 13, 8, 2, 2, SeaLantern, 3);
			AddPixelRect(world, board, 7, 2, 2, 3, DiamondOre, 2);
			AddPixelRect(world, board, 19, 13, 2, 3, EmeraldOre, 2);
			return world.AddLightPost(-34, 64, 0).AddLightPost(34, 64, 0);
		}

		[[nodiscard]] WorldConfig BuildArcadeBlocksScene()
		{
			WorldConfig world = Scene(8);
			const ArcadeBoard board{ -20, -18, 64, 20, 18, 2 };
			AddArcadeArena(world, board);
			AddPixelRect(world, board, 8, 1, 4, 1, Glass, 3);
			AddPixelRect(world, board, 10, 2, 1, 3, Glass, 3);
			AddPixelRect(world, board, 3, 13, 4, 2, MossBlock, 2);
			AddPixelRect(world, board, 7, 12, 2, 3, CherryPlanks, 2);
			AddPixelRect(world, board, 9, 14, 5, 2, GoldOre, 2);
			AddPixelRect(world, board, 14, 11, 2, 5, DiamondOre, 2);
			AddPixelRect(world, board, 16, 15, 3, 1, EmeraldOre, 2);
			return world.AddLightPost(-26, 64, -24).AddLightPost(26, 64, 24);
		}

		[[nodiscard]] WorldConfig BuildArcadeInvadersScene()
		{
			WorldConfig world = Scene(8);
			const ArcadeBoard board{};
			AddArcadeArena(world, board);
			for (int row = 0; row < 3; ++row)
			{
				for (int column = 0; column < 5; ++column)
				{
					AddSprite(world, board, 4 + (column * 4), 3 + (row * 3), { " X ", "XXX", "X X" }, row == 0 ? EmeraldOre : MossBlock, 2);
				}
			}
			AddPixelRect(world, board, 13, 15, 3, 1, SeaLantern, 3);
			AddPixel(world, board, 14, 14, SeaLantern, 3);
			AddPixel(world, board, 6, 12, Glass, 2);
			AddPixel(world, board, 18, 10, Glass, 2);
			return world.AddLightPost(-34, 64, -24).AddLightPost(34, 64, 24);
		}

		[[nodiscard]] WorldConfig BuildArcadeMazeScene()
		{
			WorldConfig world = Scene(8);
			const ArcadeBoard board{};
			AddArcadeArena(world, board);
			for (int x = 2; x < board.width - 2; x += 4)
			{
				AddPixelRect(world, board, x, 2, 1, 12, Blackstone, 2);
			}
			AddPixelRect(world, board, 3, 4, 8, 1, Blackstone, 2);
			AddPixelRect(world, board, 14, 7, 8, 1, Blackstone, 2);
			AddPixelRect(world, board, 5, 13, 18, 1, Blackstone, 2);
			AddPixel(world, board, 4, 3, GoldOre, 3);
			AddPixel(world, board, 8, 11, GoldOre, 3);
			AddPixel(world, board, 19, 5, GoldOre, 3);
			AddSprite(world, board, 12, 9, { " X ", "XXX", " X " }, SeaLantern, 4);
			AddSprite(world, board, 22, 3, { "XXX", "X X", "XXX" }, CherryLeaves, 3);
			return world.AddLightPost(-34, 64, -24).AddLightPost(34, 64, 24);
		}

		[[nodiscard]] EngineConfig ArcadeConfig(std::string title, WorldConfig world)
		{
			return EngineConfig::MinecraftDemo()
				.WithWindow(Window(std::move(title), 960, 720).EnableVSync())
				.WithScene(std::move(world))
				.WithCamera({ 0.0f, 105.0f, 58.0f }, { 0.0f, 66.0f, 0.0f })
				.WithRenderDistanceChunks(3)
				.ShowDebugOverlay(false);
		}
	}

	WindowConfig WindowConfig::Windowed(std::string title, int width, int height)
	{
		WindowConfig config{};
		config.title = std::move(title);
		config.width = width;
		config.height = height;
		config.fullscreen = false;
		return config;
	}

	WindowConfig WindowConfig::Fullscreen(std::string title)
	{
		WindowConfig config{};
		config.title = std::move(title);
		config.fullscreen = true;
		return config;
	}

	WindowConfig& WindowConfig::EnableVSync(bool enabled) noexcept
	{
		vsync = enabled;
		return *this;
	}

	WindowConfig Window(std::string title, int width, int height)
	{
		return WindowConfig::Windowed(std::move(title), width, height);
	}

	WindowConfig Fullscreen(std::string title)
	{
		return WindowConfig::Fullscreen(std::move(title));
	}

	EngineConfig EngineConfig::Default()
	{
		return {};
	}

	EngineConfig EngineConfig::DesertDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Crystal Jungle");
		config.world = WorldConfig::SizeChunks(20);
		config.camera = CameraConfig::At({ 154.0f, 82.0f, 292.0f }, { 160.0f, 52.0f, 86.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::HyperrealDesert;
		config.render_distance_chunks = 16;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::AquaModelDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Aqua Model Preview");
		config.world = WorldConfig::SizeChunks(16);
		config.camera = CameraConfig::At({ 120.0f, 82.0f, 168.0f }, { 92.0f, 58.0f, 92.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::AquaModel;
		config.render_distance_chunks = 12;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::SponzaAtriumDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Sponza Atrium Preview");
		config.world = WorldConfig::SizeChunks(20);
		config.camera = CameraConfig::At({ 176.0f, 90.0f, 210.0f }, { 128.0f, 58.0f, 116.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::SponzaAtrium;
		config.render_distance_chunks = 16;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::MinecraftDemo()
	{
		return DesertDemo();
	}

	EngineConfig EngineConfig::WorldEditDemo()
	{
		return MinecraftDemo()
			.WithWindow(Window("Voxel API - World Edit", 1280, 720).EnableVSync())
			.WithScene(BuildWorldEditDemoScene())
			.WithCamera({ 18.0f, 72.0f, 18.0f }, { 0.0f, 64.0f, 0.0f })
			.WithRenderDistanceChunks(3);
	}

	EngineConfig EngineConfig::ShowcaseDemo()
	{
		return MinecraftDemo()
			.WithWindow(Window("Voxel API - Showcase", 1600, 900).EnableVSync())
			.WithScene(BuildShowcaseDemoScene())
			.WithCamera({ 24.0f, 76.0f, 28.0f }, { 0.0f, 64.0f, 0.0f })
			.WithRenderDistanceChunks(4)
			.ShowDebugOverlay(false);
	}

	EngineConfig EngineConfig::ArcadeSnakeDemo()
	{
		return ArcadeConfig("Voxel Arcade - Snake Trail", BuildArcadeSnakeScene());
	}

	EngineConfig EngineConfig::ArcadePaddleDemo()
	{
		return ArcadeConfig("Voxel Arcade - Paddle Duel", BuildArcadePaddleScene());
	}

	EngineConfig EngineConfig::ArcadeBlocksDemo()
	{
		return ArcadeConfig("Voxel Arcade - Falling Blocks", BuildArcadeBlocksScene());
	}

	EngineConfig EngineConfig::ArcadeInvadersDemo()
	{
		return ArcadeConfig("Voxel Arcade - Star Defenders", BuildArcadeInvadersScene());
	}

	EngineConfig EngineConfig::ArcadeMazeDemo()
	{
		return ArcadeConfig("Voxel Arcade - Neon Maze", BuildArcadeMazeScene());
	}

	EngineConfig DesertDemo()
	{
		return EngineConfig::DesertDemo();
	}

	EngineConfig AquaModelDemo()
	{
		return EngineConfig::AquaModelDemo();
	}

	EngineConfig SponzaAtriumDemo()
	{
		return EngineConfig::SponzaAtriumDemo();
	}

	EngineConfig MinecraftDemo()
	{
		return DesertDemo();
	}

	EngineConfig WorldEditDemo()
	{
		return EngineConfig::WorldEditDemo();
	}

	EngineConfig ShowcaseDemo()
	{
		return EngineConfig::ShowcaseDemo();
	}

	EngineConfig ArcadeSnakeDemo()
	{
		return EngineConfig::ArcadeSnakeDemo();
	}

	EngineConfig ArcadePaddleDemo()
	{
		return EngineConfig::ArcadePaddleDemo();
	}

	EngineConfig ArcadeBlocksDemo()
	{
		return EngineConfig::ArcadeBlocksDemo();
	}

	EngineConfig ArcadeInvadersDemo()
	{
		return EngineConfig::ArcadeInvadersDemo();
	}

	EngineConfig ArcadeMazeDemo()
	{
		return EngineConfig::ArcadeMazeDemo();
	}

	EngineConfig DefaultConfig()
	{
		return EngineConfig::Default();
	}

	EngineConfig& EngineConfig::WithWindow(WindowConfig value) noexcept
	{
		window = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithWindow(std::string title, int width, int height)
	{
		return WithWindow(Window(std::move(title), width, height));
	}

	EngineConfig& EngineConfig::WithWorld(WorldConfig value) noexcept
	{
		world = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithScene(WorldConfig value) noexcept
	{
		return WithWorld(std::move(value));
	}

	EngineConfig& EngineConfig::WithAssets(AssetCatalog value) noexcept
	{
		assets = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithMaterials(MaterialLibrary value) noexcept
	{
		materials = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithSceneGraph(SceneGraph value) noexcept
	{
		scene_graph = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithCamera(CameraConfig value) noexcept
	{
		camera = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithCamera(Vec3 position, Vec3 look_at) noexcept
	{
		return WithCamera(Camera(position, look_at));
	}

	EngineConfig& EngineConfig::UseGraphicsApi(GraphicsApi value) noexcept
	{
		graphics_api = value;
		return *this;
	}

	EngineConfig& EngineConfig::UseVulkan() noexcept
	{
		return UseGraphicsApi(GraphicsApi::Vulkan);
	}

	EngineConfig& EngineConfig::UseOpenGLCompatibility() noexcept
	{
		return UseGraphicsApi(GraphicsApi::OpenGLCompatibility);
	}

	EngineConfig& EngineConfig::UseDirectX12() noexcept
	{
		return UseGraphicsApi(GraphicsApi::DirectX12);
	}

	EngineConfig& EngineConfig::WithDemoScene(DemoScene value) noexcept
	{
		demo_scene = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithWorldSizeChunks(int value) noexcept
	{
		world.size_chunks = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithRenderDistanceChunks(int value) noexcept
	{
		render_distance_chunks = value;
		return *this;
	}

	EngineConfig& EngineConfig::ShowDebugOverlay(bool enabled) noexcept
	{
		show_debug_overlay = enabled;
		return *this;
	}

	EngineConfig& EngineConfig::HideDebugOverlay() noexcept
	{
		return ShowDebugOverlay(false);
	}

	EngineConfig& EngineConfig::OnUpdate(UpdateCallback callback) noexcept
	{
		on_update = std::move(callback);
		return *this;
	}

	EngineConfig& EngineConfig::OnDiagnostics(DiagnosticsCallback callback) noexcept
	{
		on_diagnostics = std::move(callback);
		return *this;
	}

	EngineConfig& EngineConfig::OnLog(LogCallback callback) noexcept
	{
		on_log = std::move(callback);
		return *this;
	}

	class Engine::Impl
	{
	public:
		explicit Impl(const EngineConfig& config)
			: validation_issues_(detail::DefaultEngineConfigValidator().Validate(config)),
			  on_log_(config.on_log)
		{
			if (validation_issues_.empty())
			{
				runtime_ = detail::DefaultEngineRuntimeFactory().Create(
					detail::DefaultEngineConfigTranslator().Translate(config));
			}
		}

		[[nodiscard]] int Run()
		{
			if (!validation_issues_.empty())
			{
				if (on_log_)
				{
					for (const std::string& issue : validation_issues_)
					{
						on_log_("Invalid EngineConfig: " + issue);
					}
				}
				return -1;
			}
			return runtime_ ? runtime_->Run() : -1;
		}

	private:
		std::vector<std::string> validation_issues_;
		LogCallback on_log_;
		std::unique_ptr<detail::IEngineRuntime> runtime_;
	};

	Engine::Engine(EngineConfig config)
		: impl_(std::make_unique<Impl>(config))
	{
	}

	Engine::~Engine() = default;

	Engine::Engine(Engine&&) noexcept = default;

	Engine& Engine::operator=(Engine&&) noexcept = default;

	int Engine::Run()
	{
		return impl_->Run();
	}

	int Run(EngineConfig config)
	{
		Engine engine{ std::move(config) };
		return engine.Run();
	}

	int Run(WorldConfig world)
	{
		return Run(DesertDemo().WithWorld(std::move(world)));
	}

	int RunScene(WorldConfig scene)
	{
		return Run(std::move(scene));
	}

	int RunDesertDemo()
	{
		return Run(DesertDemo());
	}

	int RunAquaModelDemo()
	{
		return Run(AquaModelDemo());
	}

	int RunSponzaAtriumDemo()
	{
		return Run(SponzaAtriumDemo());
	}

	int RunMinecraftDemo()
	{
		return RunDesertDemo();
	}

	int RunWorldEditDemo()
	{
		return Run(WorldEditDemo());
	}

	int RunShowcaseDemo()
	{
		return Run(ShowcaseDemo());
	}

	int RunArcadeSnakeDemo()
	{
		return Run(ArcadeSnakeDemo());
	}

	int RunArcadePaddleDemo()
	{
		return Run(ArcadePaddleDemo());
	}

	int RunArcadeBlocksDemo()
	{
		return Run(ArcadeBlocksDemo());
	}

	int RunArcadeInvadersDemo()
	{
		return Run(ArcadeInvadersDemo());
	}

	int RunArcadeMazeDemo()
	{
		return Run(ArcadeMazeDemo());
	}
}
