#include "voxel/Engine.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <string>

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
}
