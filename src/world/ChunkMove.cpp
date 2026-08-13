#include "Chunk.h"

#include <algorithm>
#include <utility>

Chunk::Chunk(Chunk&& other) noexcept
	: mesh_(std::move(other.mesh_)),
	  chunk_x_(other.chunk_x_),
	  chunk_z_(other.chunk_z_),
	  mesh_revision_(other.mesh_revision_),
	  is_mesh_built_(other.is_mesh_built_),
	  is_generated_(other.is_generated_),
	  is_mesh_build_queued_(other.is_mesh_build_queued_),
	  has_procedural_terrain_(other.has_procedural_terrain_),
	  has_authored_edits_(other.has_authored_edits_)
{
	std::copy(
		&other.blocks_[0][0][0],
		&other.blocks_[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH),
		&blocks_[0][0][0]);
	other.is_mesh_built_ = false;
}

Chunk& Chunk::operator=(Chunk&& other) noexcept
{
	if (this == &other) return *this;
	mesh_ = std::move(other.mesh_);
	chunk_x_ = other.chunk_x_;
	chunk_z_ = other.chunk_z_;
	mesh_revision_ = other.mesh_revision_;
	is_mesh_built_ = other.is_mesh_built_;
	is_generated_ = other.is_generated_;
	is_mesh_build_queued_ = other.is_mesh_build_queued_;
	has_procedural_terrain_ = other.has_procedural_terrain_;
	has_authored_edits_ = other.has_authored_edits_;
	std::copy(
		&other.blocks_[0][0][0],
		&other.blocks_[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH),
		&blocks_[0][0][0]);
	other.is_mesh_built_ = false;
	return *this;
}
