		void ResetDemoWorld(ve::world::World& world)
		{
			const ve::world::WorldMetrics metrics = world.Metrics();
			world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ metrics.worldSizeInChunks });
		}

		void FillEllipsoid(ve::world::World& world,
			const DemoBounds& bounds,
			int cx,
			int cy,
			int cz,
			int rx,
			int ry,
			int rz,
			BlockId block)
		{
			for (int dx = -rx; dx <= rx; ++dx)
			{
				for (int dy = -ry; dy <= ry; ++dy)
				{
					for (int dz = -rz; dz <= rz; ++dz)
					{
						const float nx = static_cast<float>(dx) / static_cast<float>(std::max(rx, 1));
						const float ny = static_cast<float>(dy) / static_cast<float>(std::max(ry, 1));
						const float nz = static_cast<float>(dz) / static_cast<float>(std::max(rz, 1));
						if ((nx * nx) + (ny * ny) + (nz * nz) <= 1.0f)
						{
							SetBlock(world, bounds, cx + dx, cy + dy, cz + dz, block);
						}
					}
				}
			}
		}

		[[nodiscard]] std::string Lowercase(std::string value)
		{
			std::ranges::transform(value, value.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
			return value;
		}

		[[nodiscard]] bool IsModelExtension(const std::filesystem::path& path)
		{
			const std::string extension = Lowercase(path.extension().string());
			return extension == ".obj" || extension == ".fbx" || extension == ".gltf" || extension == ".glb" ||
				extension == ".dae" || extension == ".stl" || extension == ".ply" || extension == ".3ds" ||
				extension == ".blend" || extension == ".x";
		}

		[[nodiscard]] std::filesystem::path AssetRoot()
		{
#if defined(ROOT_DIR)
			return std::filesystem::path{ ROOT_DIR } / "assets" / "models";
#else
			return std::filesystem::current_path() / "assets" / "models";
#endif
		}

		[[nodiscard]] std::optional<std::filesystem::path> FindModelAsset(std::string_view keyword)
		{
			const std::filesystem::path root = AssetRoot();
			if (!std::filesystem::exists(root)) return std::nullopt;
			const std::string needle = Lowercase(std::string{ keyword });
			for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(root))
			{
				if (!entry.is_regular_file()) continue;
				const std::filesystem::path path = entry.path();
				if (!IsModelExtension(path)) continue;
				const std::string name = Lowercase(path.filename().string());
				const std::string parent = Lowercase(path.parent_path().filename().string());
				if (name.find(needle) != std::string::npos || parent.find(needle) != std::string::npos) return path;
			}
			return std::nullopt;
		}

		[[nodiscard]] std::uint64_t VoxelKey(int x, int y, int z) noexcept
		{
			return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x)) << 40u) ^
				(static_cast<std::uint64_t>(static_cast<std::uint32_t>(y)) << 20u) ^
				static_cast<std::uint64_t>(static_cast<std::uint32_t>(z));
		}

		[[nodiscard]] std::uint32_t StableStringHash(std::string_view value) noexcept
		{
			std::uint32_t hash = 2166136261u;
			for (char ch : value)
			{
				hash ^= static_cast<std::uint8_t>(ch);
				hash *= 16777619u;
			}
			return hash;
		}

		template <std::size_t Count>
		[[nodiscard]] BlockId PickBlock(const std::array<BlockId, Count>& palette, std::uint32_t hash) noexcept
		{
			return palette[hash % Count];
		}
