#include "voxel/Engine.h"

#include "EngineWorldSerializationValidation.h"
#include "WorldSerializationHelpers.h"

#include <fstream>
#include <string>

namespace voxel
{
	using detail::SaveWorldTerrainConfig;
	using detail::ToSerializedBlock;

	bool SaveWorldConfig(const WorldConfig& world, const std::string& path)
	{
		if (!detail::IsWorldConfigSerializable(world)) return false;

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
		return file.good();
	}

}
