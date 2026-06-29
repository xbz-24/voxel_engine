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
