		struct ImportedTexture
		{
			int width = 0;
			int height = 0;
			std::vector<std::uint8_t> pixels;
		};

		using TextureCache = std::unordered_map<std::string, ImportedTexture>;

		[[nodiscard]] bool IsLoaded(const ImportedTexture& texture) noexcept
		{
			return texture.width > 0 && texture.height > 0 && !texture.pixels.empty();
		}

		[[nodiscard]] const ImportedTexture* CachedTexture(TextureCache& cache, const std::filesystem::path& path)
		{
			if (path.empty()) return nullptr;
			const std::string key = path.lexically_normal().string();
			if (const auto existing = cache.find(key); existing != cache.end())
			{
				return IsLoaded(existing->second) ? &existing->second : nullptr;
			}

			ImportedTexture texture;
			if (std::filesystem::exists(path))
			{
				int width = 0;
				int height = 0;
				int channels = 0;
				stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
				if (data != nullptr && width > 0 && height > 0)
				{
					const std::size_t byte_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u;
					texture.width = width;
					texture.height = height;
					texture.pixels.assign(data, data + byte_count);
				}
				stbi_image_free(data);
			}

			const auto inserted = cache.emplace(key, std::move(texture));
			return IsLoaded(inserted.first->second) ? &inserted.first->second : nullptr;
		}

		[[nodiscard]] float NormalizedHeight(const glm::vec3& point, const glm::vec3& minimum, const glm::vec3& maximum) noexcept
		{
			const float height = std::max(maximum.y - minimum.y, 0.0001f);
			return std::clamp((point.y - minimum.y) / height, 0.0f, 1.0f);
		}

		[[nodiscard]] bool HasUsefulMaterialColor(const glm::vec4& color) noexcept
		{
			const float white_distance = std::abs(color.r - 1.0f) + std::abs(color.g - 1.0f) + std::abs(color.b - 1.0f);
			const float black_distance = color.r + color.g + color.b;
			const float saturation = std::max({ color.r, color.g, color.b }) - std::min({ color.r, color.g, color.b });
			return color.a > 0.05f && white_distance > 0.20f && black_distance > 0.08f && saturation > 0.025f;
		}

		[[nodiscard]] bool HasUsefulTextureColor(ve::blocks::SolidBlockColor color) noexcept
		{
			const float white_distance = std::abs(color.r - 1.0f) + std::abs(color.g - 1.0f) + std::abs(color.b - 1.0f);
			const float black_distance = color.r + color.g + color.b;
			return color.a > 0.05f && white_distance > 0.08f && black_distance > 0.06f;
		}

		[[nodiscard]] ve::blocks::SolidBlockColor SampleTextureColor(const ImportedTexture& texture, glm::vec2 uv) noexcept
		{
			uv.x -= std::floor(uv.x);
			uv.y -= std::floor(uv.y);
			const int x = std::clamp(static_cast<int>(uv.x * static_cast<float>(texture.width)), 0, texture.width - 1);
			const int y = std::clamp(static_cast<int>((1.0f - uv.y) * static_cast<float>(texture.height)), 0, texture.height - 1);
			const std::size_t index = ((static_cast<std::size_t>(y) * static_cast<std::size_t>(texture.width)) + static_cast<std::size_t>(x)) * 4u;
			return {
				static_cast<float>(texture.pixels[index]) / 255.0f,
				static_cast<float>(texture.pixels[index + 1u]) / 255.0f,
				static_cast<float>(texture.pixels[index + 2u]) / 255.0f,
				static_cast<float>(texture.pixels[index + 3u]) / 255.0f
			};
		}
