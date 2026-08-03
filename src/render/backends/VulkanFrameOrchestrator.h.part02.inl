
		/** Records the GPU chunk renderer and overlay command stream. */
		[[nodiscard]] bool RecordGpuCommandBuffer(
			VkCommandBuffer command_buffer,
			std::uint32_t image_index,
			std::size_t frame_index,
			float elapsed_seconds,
			const Camera& camera);
		/** Updates GPU chunk meshes, records the GPU path, and presents one frame. */
		[[nodiscard]] bool DrawGpuFrame(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			VulkanOverlaySettings& overlay_settings,
			const VulkanGpuFrameControls& controls);

		/** Rasterizes and presents one frame through the CPU voxel path. */
		[[nodiscard]] bool DrawSoftwareFrame(const ve::world::World& world,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			const VulkanFrameInput& input);

		/** Blocks until all tracked frames in flight are idle. */
		[[nodiscard]] bool WaitForAllInFlightFrames() const;

		/** Reads GPU timestamp results from the completed frame when available. */
		void CaptureCompletedGpuTiming(std::size_t frame_index, VulkanFrameTiming& timing) const;

		/** Releases intermediate copy images tied to the current swapchain format. */
		void ReleaseIntermediateImages();

		/** @return Memory type index matching the requested Vulkan memory properties. */
		[[nodiscard]] static std::uint32_t FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties);

		static constexpr std::size_t kFramesInFlight = 2;

		/** Input-driven toggles consumed while building a GPU frame. */
		struct VulkanGpuFrameControls
		{
			bool overlay_enabled = false;
			bool toggle_overlay = false;
		};

		/** Per-frame Vulkan objects reused across the frames-in-flight ring. */
		struct FrameResources
		{
			VulkanUploadBuffer upload_buffer;
			VkImage intermediate_image = VK_NULL_HANDLE;
			VkDeviceMemory intermediate_image_memory = VK_NULL_HANDLE;
			VkImageLayout intermediate_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkCommandBuffer command_buffer = VK_NULL_HANDLE;
			VkSemaphore image_available = VK_NULL_HANDLE;
			VkSemaphore render_finished = VK_NULL_HANDLE;
			VkFence in_flight = VK_NULL_HANDLE;
			bool timestamp_query_valid = false;
		};

		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkQueryPool timestamp_query_pool_ = VK_NULL_HANDLE;
		float timestamp_period_ns_ = 0.0f;
		std::array<FrameResources, kFramesInFlight> frames_{};
		VulkanGpuChunkRenderer gpu_chunk_renderer_;
		VulkanImGuiOverlay imgui_overlay_;
		VulkanSoftwareVoxelRasterizer rasterizer_;
		VulkanFrameTiming previous_frame_timing_{};
		VulkanSoftwareRasterizerSettings software_rasterizer_settings_{};
		VkExtent2D intermediate_extent_{};
		VkFormat intermediate_format_ = VK_FORMAT_UNDEFINED;
		VkFilter upscale_filter_ = VK_FILTER_NEAREST;
		ve::core::DynamicArray<VkImageLayout> image_layouts_;
		ve::core::DynamicArray<VkFence> images_in_flight_;
		std::size_t current_frame_ = 0;
		double shader_elapsed_seconds_ = 0.0;
		bool imgui_overlay_enabled_ = true;
		bool logged_first_frame_ = false;
	};
}
