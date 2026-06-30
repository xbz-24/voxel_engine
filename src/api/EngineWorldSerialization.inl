	bool SaveWorldConfig(const WorldConfig& world, const std::string& path)
	{
		std::ofstream file(path, std::ios::trunc);
		if (!file) return false;

		file << "voxel-world-config 1\n";
		file << "size " << world.size_chunks << '\n';
		SaveWorldTerrainConfig(file, world);
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
			else if (TryLoadWorldTerrainCommand(world, command, file))
			{
				continue;
			}
			else if (command == "set")
			{
				int block_x = 0;
				int block_y = 0;
				int block_z = 0;
				int serialized_block = 0;
				file >> block_x >> block_y >> block_z >> serialized_block;
				world.SetBlock(block_x, block_y, block_z, FromSerializedBlock(serialized_block));
			}
			else if (command == "fill")
			{
				int first_block_x = 0;
				int first_block_y = 0;
				int first_block_z = 0;
				int second_block_x = 0;
				int second_block_y = 0;
				int second_block_z = 0;
				int serialized_block = 0;
				file >> first_block_x >> first_block_y >> first_block_z >>
					second_block_x >> second_block_y >> second_block_z >>
					serialized_block;
				world.FillBox(
					first_block_x,
					first_block_y,
					first_block_z,
					second_block_x,
					second_block_y,
					second_block_z,
					FromSerializedBlock(serialized_block));
			}
			else
			{
				std::string ignored_line;
				std::getline(file, ignored_line);
			}
		}
		return world;
	}
