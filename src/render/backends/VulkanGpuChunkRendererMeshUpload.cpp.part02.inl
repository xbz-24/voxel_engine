		const double rebuild_ms = std::chrono::duration<double, std::milli>(rebuild_end - rebuild_start).count();
		const double upload_ms = std::chrono::duration<double, std::milli>(upload_end - upload_start).count();
		mesh_stats_ = VulkanGpuChunkMeshStats{
			rebuild_ms,
			upload_ms,
			RenderElementCount(vertices.size()),
			index_count_,
			shadow_index_count_,
			last_rebuilt_chunk_count_,
			RenderElementCount(cached_chunk_meshes_.size()),
			vertex_buffer_capacity_bytes_,
			index_buffer_capacity_bytes_,
			shadow_index_buffer_capacity_bytes_
		};
		VE_LOG_CATEGORY_DEBUG(ve::log::category::Render, "Rebuilt Vulkan world mesh: " + std::to_string(index_count_) +
			" indices, " + std::to_string(shadow_index_count_) + " shadow indices, " +
			std::to_string(last_rebuilt_chunk_count_) + "/" +
			std::to_string(cached_chunk_meshes_.size()) + " chunks rebuilt in " +
			std::to_string(rebuild_ms + upload_ms) + " ms");
		return true;
	}
	bool VulkanGpuChunkRenderer::NeedsWorldMeshUpdate(const ve::world::World& world) const noexcept
	{
		return !mesh_valid_ || mesh_revision_ != world.Revision();
	}
}
