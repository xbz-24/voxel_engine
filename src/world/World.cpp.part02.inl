	std::unique_ptr<ve::rendering::RenderMesh> World::CreateChunkRenderMeshResource() const
	{
		return active_render_backend_ != nullptr ? active_render_backend_->CreateMeshResource() : nullptr;
	}

	/**
	 * Returns loaded chunk and capacity diagnostics.
	 *
	 * @return Snapshot of world size and chunk storage counts.
	 */
	WorldMetrics World::Metrics() const noexcept
	{
		std::size_t chunksNeedingMeshBuild = 0;
		std::size_t chunksWithQueuedMeshBuild = 0;
		for (const Chunk& chunk : _chunks)
		{
			if (chunk.NeedsMeshBuild()) chunksNeedingMeshBuild++;
			if (chunk.HasPendingMeshBuildReservation()) chunksWithQueuedMeshBuild++;
		}
		return WorldMetrics{
			_worldSize,
			_chunks.size(),
			_chunks.capacity(),
			chunksNeedingMeshBuild,
			chunksWithQueuedMeshBuild,
			0,
			0,
			0,
			_chunks.capacity() * sizeof(Chunk),
			_levelSpawn.MemoryCapacityBytes(),
			_levelSpawn.MemoryBytesUsed(),
			_pendingEvents.size(),
			chunk_storage_policy_
		};
	}

	std::uint64_t World::Revision() const noexcept
	{
		return _revision;
	}

	std::uint64_t World::ChunkStorageRevision() const noexcept
	{
		return _chunkStorageRevision;
	}

	std::span<const Chunk> World::Chunks() const noexcept
	{
		return _chunks;
	}

	std::span<const DirtyChunkMetadata> World::DirtyChunks() const noexcept
	{
		return dirty_chunks_;
	}

	void World::RecordDirtyChunk(const Chunk& chunk)
	{
		const auto existing_metadata = std::ranges::find_if(dirty_chunks_,
			[&chunk](const DirtyChunkMetadata& metadata) noexcept {
				return metadata.chunk_x == chunk.GetChunkX() && metadata.chunk_z == chunk.GetChunkZ();
			});
		const DirtyChunkMetadata metadata{
			chunk.GetChunkX(),
			chunk.GetChunkZ(),
			chunk.MeshRevision(),
			chunk.HasAuthoredEdits()
		};
		if (existing_metadata != dirty_chunks_.end())
		{
			*existing_metadata = metadata;
			return;
		}
		dirty_chunks_.push_back(metadata);
	}

	void World::MarkChunkDirty(Chunk& chunk)
	{
		chunk.MarkDirty();
		RecordDirtyChunk(chunk);
	}
}
