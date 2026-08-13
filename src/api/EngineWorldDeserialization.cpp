#include "voxel/Engine.h"

#include "WorldSerializationHelpers.h"

#include <fstream>
#include <string>

namespace voxel
{
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
			else if (detail::TryLoadWorldTerrainCommand(world, command, file))
			{
				continue;
			}
			else if (command == "set")
			{
				int x = 0, y = 0, z = 0, serialized_block = 0;
				file >> x >> y >> z >> serialized_block;
				world.SetBlock(x, y, z, detail::FromSerializedBlock(serialized_block));
			}
			else if (command == "fill")
			{
				int x1 = 0, y1 = 0, z1 = 0;
				int x2 = 0, y2 = 0, z2 = 0;
				int serialized_block = 0;
				file >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> serialized_block;
				world.FillBox(x1, y1, z1, x2, y2, z2,
					detail::FromSerializedBlock(serialized_block));
			}
			else
			{
				std::string ignored_line;
				std::getline(file, ignored_line);
			}
		}
		return world;
	}
}
