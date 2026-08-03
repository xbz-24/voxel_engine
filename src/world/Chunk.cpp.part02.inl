	has_authored_edits_ = true;
	MarkDirty();
	return true;
}
void Chunk::MarkDirty()
{
	++mesh_revision_;
	is_mesh_built_ = false;
	is_mesh_build_queued_ = false;
}
std::uint64_t Chunk::MeshRevision() const noexcept
{
	return mesh_revision_;
}
bool Chunk::ContainsLocalBlock(int local_block_x, int local_block_y, int local_block_z) const
{
	return local_block_x >= 0 && local_block_x < CHUNK_WIDTH &&
		local_block_y >= 0 && local_block_y < CHUNK_HEIGHT &&
		local_block_z >= 0 && local_block_z < CHUNK_DEPTH;
}
