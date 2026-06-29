namespace ve::engine
{
	namespace
	{
		[[nodiscard]] bool IsTunnelMaterial(ve::blocks::BlockId block) noexcept
		{
			using enum ve::blocks::BlockId;
			static constexpr std::array<ve::blocks::BlockId, 16> materials{ {
				Grass, Dirt, Stone, Cobblestone, Gravel, MossBlock, MossyCobblestone, Granite, Andesite, Diorite,
				Deepslate, Sand, Sandstone, RedSand, RedSandstone, Terracotta
			} };
			return std::ranges::find(materials, block) != materials.end();
		}

		[[nodiscard]] bool SetBlockIfChanged(ve::world::World& world, const glm::ivec3& position, ve::blocks::BlockId block)
		{
			return world.GetBlock(position) != block && world.SetBlock(position, block);
		}

		[[nodiscard]] bool CarveRoundedHole(ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const glm::ivec3& center)
		{
			bool changed = false;
			constexpr int radius = 3;
			constexpr int depth = 6;
			for (int dx = -radius; dx <= radius; ++dx)
			{
				for (int dz = -radius; dz <= radius; ++dz)
				{
					const float horizontal = static_cast<float>((dx * dx) + (dz * dz)) / static_cast<float>(radius * radius);
					if (horizontal > 1.10f) continue;
					for (int dy = 0; dy >= -depth; --dy)
					{
						const float vertical = static_cast<float>(dy * dy) / static_cast<float>(depth * depth);
						if (horizontal + (vertical * 0.72f) > 1.0f) continue;
						const glm::ivec3 position = center + glm::ivec3{ dx, dy, dz };
						const ve::blocks::BlockId block = world.GetBlock(position);
						if (!block_registry.IsSolid(block) || !IsTunnelMaterial(block)) continue;
						changed = SetBlockIfChanged(world, position, ve::blocks::BlockId::Air) || changed;
					}
				}
			}

			for (int dx = -(radius + 1); dx <= radius + 1; ++dx)
			{
				for (int dz = -(radius + 1); dz <= radius + 1; ++dz)
				{
					const int distance_sq = (dx * dx) + (dz * dz);
					if (distance_sq < (radius * radius) || distance_sq > ((radius + 1) * (radius + 1))) continue;
					const glm::ivec3 rim = center + glm::ivec3{ dx, 0, dz };
					const ve::blocks::BlockId block = world.GetBlock(rim);
					if (!IsTunnelMaterial(block)) continue;
					const ve::blocks::BlockId rim_block = ((dx + dz) & 1) == 0 ? ve::blocks::BlockId::MossBlock : ve::blocks::BlockId::MossyCobblestone;
					changed = SetBlockIfChanged(world, rim, rim_block) || changed;
				}
			}
			return changed;
		}

		void ConsumeFlyToggle(const ve::input::InputSnapshot& input, ve::gameplay::RuntimeSettings& settings, bool& was_pressed)
		{
			if (!ve::input::WasPressed(input, ve::input::Key::F, was_pressed)) return;
			settings.isFlying = !settings.isFlying;
			settings.verticalVelocity = 0.0f;
		}

		void ConsumeRenderDistanceAdjustment(const ve::input::InputSnapshot& input,
			ve::gameplay::RuntimeSettings& settings,
			ve::input::Key key,
			int amount,
			bool& was_pressed)
		{
			if (ve::input::WasPressed(input, key, was_pressed))
			{
				settings.renderDistanceChunks = ve::gameplay::ClampRenderDistanceChunks(settings.renderDistanceChunks + amount);
			}
		}
	}

