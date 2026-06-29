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

			for (int z = min_z; z <= max_z; ++z)
			{
				for (int y = min_y; y <= max_y; ++y)
				{
					for (int x = min_x; x <= max_x; ++x)
					{
						(void)world.SetBlock(x, y, z, edit.block);
					}
				}
			}
		}
	}
