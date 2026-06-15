#pragma once

#include "Block.h"
#include "PbrMaterial.h"

namespace ve::blocks
{
	/**
	 * Builds default physically based material values for a block.
	 *
	 * @param id Block id that needs material metadata.
	 * @return PBR material tuned for the block category.
	 */
	ve::rendering::PbrMaterial DefaultPbrMaterialForBlock(BlockId id) noexcept;
}
