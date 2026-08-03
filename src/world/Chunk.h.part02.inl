	/**
	 * Builds a GPU mesh containing visible chunk faces.
	 *
	 * @param block_registry Registry used to resolve texture ids.
	 * @param neighbors Adjacent chunks used for border occlusion.
	 */
	void BuildMesh(const ve::blocks::BlockRegistry& block_registry, const ve::world::mesh::NeighborChunks& neighbors);

	/**
	 * Uploads finished CPU mesh data to this chunk's GPU buffer.
	 *
	 * @param mesh_build_result CPU vertices and texture batches to move/upload.
	 */
	void UploadMesh(ve::world::mesh::ChunkMeshBuildResult mesh_build_result);

	/** @return Backend-neutral uploaded chunk mesh resource. */
	const ve::rendering::ChunkGpuMesh& RenderMesh() const noexcept;

	/**
	 * Returns a read-only mesh input view over this chunk's block storage.
	 *
	 * @return Non-owning view valid while this chunk is alive and unchanged.
	 */
	ve::world::mesh::ChunkMeshInput CreateMeshInput() const noexcept;

	/**
	 * Reports whether this chunk needs a fresh mesh.
	 *
	 * @return True when terrain/block changes invalidated the GPU mesh.
	 */
	bool NeedsMeshBuild() const noexcept;

	/// Returns the chunk-grid X coordinate.
	int GetChunkX() const noexcept;

	/// Returns the chunk-grid Z coordinate.
	int GetChunkZ() const noexcept;

	/// Reads a block id, treating out-of-bounds as air.
	ve::blocks::BlockId GetBlock(int local_block_x, int local_block_y, int local_block_z) const;

	/// Writes a block id and marks the mesh dirty when the block changes.
	bool SetBlock(int local_block_x, int local_block_y, int local_block_z, ve::blocks::BlockId block_id);

	/// Marks the cached mesh as dirty so it is rebuilt next draw.
	void MarkDirty();

	/** @return Monotonic revision used to reject stale async mesh outputs. */
	[[nodiscard]] std::uint64_t MeshRevision() const noexcept;

private:
	ve::world::terrain::BlockStorage blocks_;
	ve::rendering::ChunkGpuMesh mesh_;
	int chunk_x_;
	int chunk_z_;
	std::uint64_t mesh_revision_;
	bool is_mesh_built_;
	bool is_generated_;
	bool is_mesh_build_queued_;
	bool has_procedural_terrain_;
	bool has_authored_edits_;

	/// Checks if a local coordinate belongs to this chunk.
	bool ContainsLocalBlock(int local_block_x, int local_block_y, int local_block_z) const;
};
