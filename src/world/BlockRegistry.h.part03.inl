		const ve::rendering::PbrMaterial& MaterialFor(BlockId id) const;

		/** @param id Block id to inspect. @return Gameplay/collision metadata for the block. */
		const BlockGameplayProperties& GameplayFor(BlockId id) const;

		/** @param id Block id to inspect. @return True when the block stops player/world movement. */
		bool BlocksMovement(BlockId id) const;

		/** @param id Block id to inspect. @return True when the block should not fully occlude rendering/light. */
		bool IsTransparent(BlockId id) const;

		/** @param id Block id to inspect. @return True when the block should emit visible geometry. */
		bool IsRenderable(BlockId id) const;

		/** @param id Block id to inspect. @return True when the block hides a neighboring face. */
		bool OccludesNeighborFaces(BlockId id) const;

	private:
		const BlockType& FallbackBlock() const noexcept;

		std::unordered_map<BlockId, BlockType> _blocks;
	};

	/** @param id Block id to inspect. @return True when the built-in id can be stored in chunks. */
	[[nodiscard]] bool IsUsableBlockId(BlockId id) noexcept;
}
