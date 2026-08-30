#include "BlockRegistry.h"

#include "BlockRegistryLoading.h"

#include <utility>

namespace ve::blocks
{
	bool IsUsableBlockId(BlockId id) noexcept
	{
		return IsStoredBlockId(id);
	}

	bool BlockRegistry::Register(BlockType block_type)
	{
		if (!IsUsableBlockId(block_type.id)) return false;
		_blocks.insert_or_assign(block_type.id, std::move(block_type));
		return true;
	}

	bool BlockRegistry::Contains(BlockId id) const
	{
		return _blocks.find(id) != _blocks.end();
	}

	std::size_t BlockRegistry::RegisteredBlockCount() const noexcept
	{
		return _blocks.size();
	}

	const BlockType& BlockRegistry::Get(BlockId id) const
	{
		const auto block_iterator = _blocks.find(id);
		return block_iterator != _blocks.end() ? block_iterator->second : FallbackBlock();
	}

	const BlockType& BlockRegistry::FallbackBlock() const noexcept
	{
		const auto air_iterator = _blocks.find(BlockId::Air);
		if (air_iterator != _blocks.end()) return air_iterator->second;

		static const BlockType fallback_air_block{
			BlockId::Air,
			"Air",
			false,
			detail::EmptyFaces(),
			ve::rendering::PbrMaterial{},
			BlockGameplayProperties{}
		};
		return fallback_air_block;
	}
}
