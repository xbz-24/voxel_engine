		/** Allocates a Vulkan buffer with caller-provided usage and memory properties. */
		[[nodiscard]] bool CreateBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) const;

		/** Allocates a host-visible buffer for staging or CPU-written data. */
		[[nodiscard]] bool CreateHostBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory) const;

		/** Stages data into a device-local buffer, growing capacity only when needed. */
		[[nodiscard]] bool UploadDeviceLocalBuffer(
			const void* source,
			VkDeviceSize byte_size,
			VkBufferUsageFlags usage,
			VkBuffer& buffer,
			VkDeviceMemory& memory,
			VkDeviceSize& capacity_bytes) const;

		/** Creates an image and binds device-local memory for renderer-owned resources. */
		[[nodiscard]] bool CreateDeviceImage(VkImageCreateInfo image_info, VkDeviceMemory& memory, VkImage& image) const;

		/** Copies CPU data into mapped Vulkan memory. */
		[[nodiscard]] bool CopyToDeviceBuffer(VkDeviceMemory memory, const void* source, VkDeviceSize byte_size) const;

		/** Submits a short-lived command buffer and waits for completion. */
		[[nodiscard]] bool RunImmediateCommands(void (*record)(VkCommandBuffer, void*), void* user_data) const;

		/** Loads a SPIR-V shader file and creates a Vulkan shader module. */
		[[nodiscard]] VkShaderModule CreateShaderModule(const std::filesystem::path& path) const;

		/** Releases resources that must be rebuilt when the swapchain changes. */
		void ReleaseSwapchainResources();

		/** Releases uploaded mesh buffers and resets mesh buffer capacities. */
		void ReleaseMeshBuffers();

		/** Releases block texture images, samplers, and descriptor resources. */
		void ReleaseTextureResources();

		/** Releases pipeline, pipeline layout, and render pass resources. */
		void ReleasePipelineResources();

		/** Releases render passes after every dependent framebuffer has been destroyed. */
		void ReleaseRenderPassResources();

		/** Releases per-frame shader descriptors and uniform buffers. */
		void ReleaseShaderFrameResources();

		/** Rebuilds the flattened visible mesh for all cached world chunks. */
		void RebuildMesh(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices);

		/** Clears cached chunk meshes when the world storage identity changes. */
		void ResetChunkMeshCacheForWorldStorage(const ve::world::World& world);

		/** @return Cache entry associated with a chunk coordinate pair. */
		[[nodiscard]] CachedChunkMesh& CachedMeshFor(const Chunk& chunk);

		/** Rebuilds one chunk mesh when its revision changed. */
		void RebuildChunkMesh(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry, const Chunk& chunk, CachedChunkMesh& cached_mesh) const;

		/** Appends a cached chunk mesh into the frame upload arrays. */
		void AppendCachedChunkMesh(const CachedChunkMesh& cached_mesh, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;

		/** Emits only faces that are visible from neighboring block state. */
		void AppendVisibleBlockFaces(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry, int block_x, int block_y, int block_z, ve::blocks::BlockId block, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;

		/** Emits vertices and indices for a single block face. */
		void AppendFaceMesh(const BlockFaceGeometry& face, const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry, int block_x, int block_y, int block_z, ve::blocks::BlockId block, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;

		/** @return Memory type index matching the requested Vulkan memory properties. */
		[[nodiscard]] static std::uint32_t FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties);

		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkRenderPass render_pass_ = VK_NULL_HANDLE;
		VkRenderPass shadow_render_pass_ = VK_NULL_HANDLE;
		VkDescriptorSetLayout shader_descriptor_set_layout_ = VK_NULL_HANDLE;
		VkDescriptorPool shader_descriptor_pool_ = VK_NULL_HANDLE;
		VkSampler shadow_sampler_ = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
		VkPipeline voxel_pipeline_ = VK_NULL_HANDLE;
		VkPipeline sky_pipeline_ = VK_NULL_HANDLE;
		VkPipeline shadow_pipeline_ = VK_NULL_HANDLE;
		VkImage depth_image_ = VK_NULL_HANDLE;
		VkDeviceMemory depth_memory_ = VK_NULL_HANDLE;
		VkImageView depth_view_ = VK_NULL_HANDLE;
		VkBuffer vertex_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory vertex_memory_ = VK_NULL_HANDLE;
		VkDeviceSize vertex_buffer_capacity_bytes_ = 0;
