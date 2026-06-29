	void EngineRuntime::ApplyConfiguredWorldEditsOnce()
	{
		if (configured_world_edits_applied_ || model_ == nullptr) return;
		if (engine_.create_info_.world_edits.empty())
		{
			configured_world_edits_applied_ = true;
			return;
		}
		if (model_->PendingWorldGenerationCount() != 0) return;

		ApplyWorldEdits(engine_.create_info_.world_edits);
		configured_world_edits_applied_ = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::World, "Applied configured public API world edits");
	}

	void EngineRuntime::ApplyWorldEdits(const std::vector<WorldBlockEdit>& edits)
	{
		if (model_ == nullptr) return;

		ve::world::World& world = model_->MutableWorld();
		for (const WorldBlockEdit& edit : edits)
		{
			const int min_x = std::min(edit.min_x, edit.max_x);
			const int min_y = std::min(edit.min_y, edit.max_y);
			const int min_z = std::min(edit.min_z, edit.max_z);
			const int max_x = std::max(edit.min_x, edit.max_x);
			const int max_y = std::max(edit.min_y, edit.max_y);
			const int max_z = std::max(edit.min_z, edit.max_z);

			for (int edit_block_z = min_z; edit_block_z <= max_z; ++edit_block_z)
			{
				for (int edit_block_y = min_y; edit_block_y <= max_y; ++edit_block_y)
				{
					for (int edit_block_x = min_x; edit_block_x <= max_x; ++edit_block_x)
					{
						(void)world.SetBlock(edit_block_x, edit_block_y, edit_block_z, edit.block);
					}
				}
			}
		}
	}
