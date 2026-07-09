		template <typename Value>
		[[nodiscard]] constexpr float SceneFloat(Value value) noexcept
		{
			return ve::core::ToFloat(value);
		}

		template <typename Value>
		[[nodiscard]] constexpr int SceneInt(Value value) noexcept
		{
			return ve::core::ToInt(value);
		}

		template <typename Value>
		[[nodiscard]] constexpr std::size_t SceneIndex(Value value) noexcept
		{
			return ve::core::ToIndex(value);
		}

		template <typename Value>
		[[nodiscard]] constexpr std::uint32_t SceneSeed(Value value) noexcept
		{
			return ve::core::ToU32(value);
		}

		template <typename Value>
		[[nodiscard]] constexpr std::uint8_t SceneByte(Value value) noexcept
		{
			return ve::core::ToU8(value);
		}

		[[nodiscard]] constexpr std::size_t SceneModuloIndex(int value, std::size_t count) noexcept
		{
			return SceneIndex(value % SceneInt(count));
		}

		[[nodiscard]] constexpr std::uint64_t SceneCoordinateBits(int value) noexcept
		{
			return ve::core::ToU64(SceneSeed(value));
		}

		[[nodiscard]] constexpr std::uint64_t PackedSceneVoxelKey(int x, int y, int z) noexcept
		{
			return (SceneCoordinateBits(x) << 40u) ^
				(SceneCoordinateBits(y) << 20u) ^
				SceneCoordinateBits(z);
		}

		[[nodiscard]] inline int RoundedSceneInt(float value) noexcept
		{
			return SceneInt(std::round(value));
		}

		[[nodiscard]] inline int CeilSceneInt(float value) noexcept
		{
			return SceneInt(std::ceil(value));
		}

		[[nodiscard]] constexpr float UnitColorChannel(std::uint8_t value) noexcept
		{
			return SceneFloat(value) / 255.0f;
		}

		[[nodiscard]] constexpr std::size_t RgbaTextureByteCount(int width, int height) noexcept
		{
			return SceneIndex(width) * SceneIndex(height) * 4u;
		}

		[[nodiscard]] constexpr std::size_t RgbaTexturePixelOffset(int x, int y, int width) noexcept
		{
			return ((SceneIndex(y) * SceneIndex(width)) + SceneIndex(x)) * 4u;
		}

		[[nodiscard]] constexpr float NormalizedRatio(int numerator, int denominator) noexcept
		{
			return SceneFloat(numerator) / SceneFloat(std::max(denominator, 1));
		}
