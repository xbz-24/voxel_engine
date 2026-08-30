#pragma once

#include "BlockDefinitionTypes.h"
#include "BlockRegistry.h"

namespace ve::blocks
{
	class BlockTextureCache;

	namespace detail
	{
		BlockFaceTextureHandles EmptyFaces() noexcept;
		BlockFaceTextureHandles LoadBuiltInFaces(
			BlockTextureCache& cache,
			const FaceTextureFiles& files);
		BlockGameplayProperties DefaultGameplayFor(const BlockDefinition& definition);
		bool CanRegisterDataDefinition(const DataBlockDefinition& definition) noexcept;
		BlockType BuildDataBlockType(
			BlockTextureCache& cache,
			const DataBlockDefinition& definition,
			BlockRegistry::TextureLoading texture_loading);
	}
}
