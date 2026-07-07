#pragma once

#include "GraphicsTypes.h"
#include "RenderMesh.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace ve::rendering
{
	class ChunkGpuMesh
	{
	public:
		/**
		 * Creates an empty chunk mesh with no GPU allocation.
		 */
		ChunkGpuMesh();

		/** Creates a chunk mesh backed by a backend-owned render resource. */
		explicit ChunkGpuMesh(std::unique_ptr<RenderMesh> render_mesh_resource) noexcept;

		/**
		 * Releases the GPU buffer owned by this mesh.
		 */
		~ChunkGpuMesh();

		ChunkGpuMesh(const ChunkGpuMesh&) = delete;
		ChunkGpuMesh& operator=(const ChunkGpuMesh&) = delete;
		/**
		 * Takes over another mesh buffer without duplicating GPU memory.
		 *
		 * @param other Source mesh being moved from.
		 */
		ChunkGpuMesh(ChunkGpuMesh&& other) noexcept;

		/**
		 * Releases this mesh and takes over another mesh buffer.
		 *
		 * @param other Source mesh being moved from.
		 * @return Reference to this mesh.
		 */
		ChunkGpuMesh& operator=(ChunkGpuMesh&& other) noexcept;

		/**
		 * Uploads CPU vertices to a static GPU buffer.
		 *
		 * @param vertices Vertex data copied into GPU memory.
		 * @param batches Texture ranges moved into this mesh.
		 */
		void Upload(const std::vector<ChunkVertex>& vertices, std::vector<ChunkMeshBatch> batches);

		/** @return Last CPU mesh payload uploaded to this chunk. */
		[[nodiscard]] MeshDescription CpuMesh() const noexcept;

		/**
		 * Draws all texture batches from the uploaded GPU buffer.
		 */
		void Draw() const;

		/**
		 * Frees the GPU buffer and clears batch metadata.
		 */
		void Release();

	private:
		std::unique_ptr<RenderMesh> _mesh;
		std::vector<ChunkVertex> cpu_vertices_;
		std::vector<ChunkMeshBatch> cpu_batches_;
	};
}
