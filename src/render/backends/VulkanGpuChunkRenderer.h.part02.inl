	private:
		enum class VertexLayout
		{
			None,
			VoxelPosition,
			Voxel
		};

		struct GraphicsPipelineSettings
		{
			VkRenderPass render_pass = VK_NULL_HANDLE;
			VertexLayout vertex_layout = VertexLayout::None;
			bool depth_test_enabled = false;
			bool depth_write_enabled = false;
			bool alpha_blending_enabled = false;
			bool color_attachment_enabled = true;
			bool depth_bias_enabled = false;
		};

		struct ShaderFrameResources
		{
			VkBuffer uniform_buffer = VK_NULL_HANDLE;
			VkDeviceMemory uniform_memory = VK_NULL_HANDLE;
			VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
			void* mapped_uniform_data = nullptr;
			VkImage shadow_image = VK_NULL_HANDLE;
			VkDeviceMemory shadow_memory = VK_NULL_HANDLE;
			VkImageView shadow_view = VK_NULL_HANDLE;
			VkFramebuffer shadow_framebuffer = VK_NULL_HANDLE;
		};

		/** CPU-side mesh cache for a world chunk and its revision. */
		struct CachedChunkMesh
		{
			int chunk_x = 0;
			int chunk_z = 0;
			std::uint64_t mesh_revision = std::numeric_limits<std::uint64_t>::max();
			std::vector<VoxelVertex> vertices;
			std::vector<std::uint32_t> indices;
		};

		/** Creates the main color pass and directional shadow depth pass. */
		[[nodiscard]] bool CreateRenderPass();

		/** Loads shaders and creates the Vulkan pipeline state. */
		[[nodiscard]] bool CreatePipeline(const std::filesystem::path& shader_directory);

		/** Creates pipeline layout state for transform constants and frame descriptors. */
		[[nodiscard]] bool CreatePipelineLayout();

		/** Creates one uniform buffer and descriptor set per frame in flight. */
		[[nodiscard]] bool CreateShaderFrameResources(std::size_t frame_resource_count);

		/** Creates the depth image, view, and framebuffer for one frame's shadow map. */
		[[nodiscard]] bool CreateShadowFrameResources(ShaderFrameResources& frame) const;

		/** Uploads dynamic camera, time, environment, and viewport data for one frame. */
		[[nodiscard]] bool UpdateShaderFrameUniforms(
			std::size_t frame_index,
			const VulkanVoxelFrameUniforms& frame_uniforms) const;

		/** Records the depth-only directional shadow pass for one frame in flight. */
		[[nodiscard]] bool RecordShadowPass(
			VkCommandBuffer command_buffer,
			std::size_t frame_index) const;

		/** Builds the graphics pipeline from compiled vertex and fragment modules. */
		[[nodiscard]] bool CreateGraphicsPipeline(
			VkShaderModule vertex_shader,
			VkShaderModule fragment_shader,
			const GraphicsPipelineSettings& settings,
			VkPipeline& output_pipeline) const;

		/** Recreates views and framebuffers tied to the active swapchain. */
		[[nodiscard]] bool CreateSwapchainResources();

		/** Creates depth image resources matching the active swapchain extent. */
		[[nodiscard]] bool CreateDepthResources();

		/** Creates one framebuffer per swapchain image view. */
		[[nodiscard]] bool CreateFramebuffers();

		/** Uploads the flattened world mesh into device-local vertex and index buffers. */
		[[nodiscard]] bool UploadMeshBuffers(
			std::span<const VoxelVertex> vertices,
			std::span<const std::uint32_t> indices,
			std::span<const std::uint32_t> shadow_indices);

