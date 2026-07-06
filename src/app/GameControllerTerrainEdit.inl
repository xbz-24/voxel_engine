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

		[[nodiscard]] bool SetBlockIfChanged(ve::world::World& world, const glm::ivec3& block_position, ve::blocks::BlockId block)
		{
			return world.GetBlock(block_position) != block && world.SetBlock(block_position, block);
		}

		[[nodiscard]] bool CarveRoundedHole(ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const glm::ivec3& tunnel_center)
		{
			bool changed = false;
			constexpr int tunnel_radius = 3;
			constexpr int tunnel_depth = 6;
			for (int offset_x = -tunnel_radius; offset_x <= tunnel_radius; ++offset_x)
			{
				for (int offset_z = -tunnel_radius; offset_z <= tunnel_radius; ++offset_z)
				{
					const float normalized_horizontal_distance =
						static_cast<float>((offset_x * offset_x) + (offset_z * offset_z)) /
						static_cast<float>(tunnel_radius * tunnel_radius);
					if (normalized_horizontal_distance > 1.10f) continue;
					for (int offset_y = 0; offset_y >= -tunnel_depth; --offset_y)
					{
						const float normalized_vertical_distance =
							static_cast<float>(offset_y * offset_y) /
							static_cast<float>(tunnel_depth * tunnel_depth);
						if (normalized_horizontal_distance + (normalized_vertical_distance * 0.72f) > 1.0f) continue;
						const glm::ivec3 block_position = tunnel_center + glm::ivec3{ offset_x, offset_y, offset_z };
						const ve::blocks::BlockId block = world.GetBlock(block_position);
						if (!block_registry.IsSolid(block) || !IsTunnelMaterial(block)) continue;
						changed = SetBlockIfChanged(world, block_position, ve::blocks::BlockId::Air) || changed;
					}
				}
			}

			for (int offset_x = -(tunnel_radius + 1); offset_x <= tunnel_radius + 1; ++offset_x)
			{
				for (int offset_z = -(tunnel_radius + 1); offset_z <= tunnel_radius + 1; ++offset_z)
				{
					const int rim_distance_squared = (offset_x * offset_x) + (offset_z * offset_z);
					if (rim_distance_squared < (tunnel_radius * tunnel_radius) ||
						rim_distance_squared > ((tunnel_radius + 1) * (tunnel_radius + 1))) continue;
					const glm::ivec3 rim_position = tunnel_center + glm::ivec3{ offset_x, 0, offset_z };
					const ve::blocks::BlockId block = world.GetBlock(rim_position);
					if (!IsTunnelMaterial(block)) continue;
					const ve::blocks::BlockId rim_block =
						((offset_x + offset_z) & 1) == 0 ?
						ve::blocks::BlockId::MossBlock :
						ve::blocks::BlockId::MossyCobblestone;
					changed = SetBlockIfChanged(world, rim_position, rim_block) || changed;
				}
			}
			return changed;
		}

		void ConsumeFlyToggle(const ve::input::InputSnapshot& input, ve::gameplay::RuntimeSettings& settings, bool& was_pressed)
		{
			if (!ve::gameplay::ConsumeGameplayAction(
				input,
				ve::gameplay::GameplayAction::ToggleFly,
				was_pressed)) return;
			settings.player.is_flying = !settings.player.is_flying;
			settings.player.vertical_velocity = 0.0f;
		}

		void ConsumeRenderDistanceAdjustment(const ve::input::InputSnapshot& input,
			ve::gameplay::RuntimeSettings& settings,
			ve::gameplay::GameplayAction action,
			int amount,
			bool& was_pressed)
		{
			if (ve::gameplay::ConsumeGameplayAction(input, action, was_pressed))
			{
				settings.renderer.render_distance_chunks = ve::gameplay::ClampRenderDistanceChunks(settings.renderer.render_distance_chunks + amount);
			}
		}
	}

