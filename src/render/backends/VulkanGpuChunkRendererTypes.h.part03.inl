				finite_non_negative(style.cloud_speed, 0.018f) },
			glm::vec4{
				finite_in_range(style.surface_detail_strength, 1.0f, 2.0f),
				finite_in_range(style.water_reflection_strength, 1.0f, 2.0f),
				finite_in_range(style.shadow_strength, 1.0f, 2.0f),
				finite_in_range(style.specular_strength, 1.0f, 2.0f) },
			glm::vec4{
				finite_in_range(style.ambient_occlusion_strength, 1.0f, 2.0f),
				finite_in_range(style.aerial_perspective_strength, 1.0f, 2.0f),
				finite_in_range(style.subsurface_scattering_strength, 1.0f, 2.0f),
				finite_in_range(style.cloud_light_absorption, 1.0f, 2.0f) },
			glm::vec4{
				finite_in_range(style.directional_shadow_strength, 0.85f, 2.0f),
				finite_in_range(style.directional_shadow_softness, 1.0f, 2.0f),
				directional_shadow_distance,
				1.0f / ve::core::ToFloat(VulkanDirectionalShadowMapResolution) },
			BuildVulkanDirectionalShadowMatrix(
				camera_position,
				camera_forward,
				sun_direction,
				directional_shadow_distance)
		};
	}

	[[nodiscard]] inline glm::length_t GlmAxis(int axis) noexcept
	{
		return ve::core::NumericCast<glm::length_t>(axis);
	}

	[[nodiscard]] inline float VertexCoordinate(int coordinate) noexcept
	{
		return ve::core::ToFloat(coordinate);
	}

	[[nodiscard]] inline float ExtentFloat(std::uint32_t value) noexcept
	{
		return ve::core::ToFloat(value);
	}

	template <typename Value>
	[[nodiscard]] inline double TimestampValueDouble(Value value) noexcept
	{
		return ve::core::ToDouble(value);
	}

	[[nodiscard]] inline std::uint32_t RenderElementCount(std::size_t count) noexcept
	{
		return ve::core::ToU32(count);
	}

	[[nodiscard]] inline std::uint32_t VertexAttributeOffset(std::size_t byte_offset) noexcept
	{
		return ve::core::ToU32(byte_offset);
	}

	[[nodiscard]] inline std::uint32_t TimestampQueryIndex(std::size_t frame_index) noexcept
	{
		return ve::core::ToU32(frame_index * 2u);
	}

	[[nodiscard]] inline std::uint32_t TimestampQueryCount(std::size_t frame_count) noexcept
	{
		return ve::core::ToU32(frame_count * 2u);
	}

	[[nodiscard]] constexpr std::size_t ChunkColumnCount(int width, int depth) noexcept
	{
		return ve::core::ToIndex(width * depth);
	}

	[[nodiscard]] constexpr std::size_t WorldColumnCount(int width_blocks, int depth_blocks) noexcept
	{
		return ve::core::ToIndex(std::max(width_blocks, 0)) *
			ve::core::ToIndex(std::max(depth_blocks, 0));
	}

	[[nodiscard]] inline VkDeviceSize VulkanByteSize(std::size_t byte_count) noexcept
	{
		return ve::core::NumericCast<VkDeviceSize>(byte_count);
	}

	[[nodiscard]] inline std::size_t HostByteSize(VkDeviceSize byte_count) noexcept
	{
		return ve::core::ToIndex(byte_count);
	}

	[[nodiscard]] inline std::uint8_t PackUnorm8(float value) noexcept
	{
