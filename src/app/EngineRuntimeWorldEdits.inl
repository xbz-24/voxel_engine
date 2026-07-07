	void EngineRuntime::ApplyConfiguredWorldEditsOnce()
	{
		if (configured_world_edits_applied_ || model_ == nullptr) return;
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		if (create_info.world_edits.empty())
		{
			configured_world_edits_applied_ = true;
			return;
		}
		if (model_->PendingWorldGenerationCount() != 0) return;

		ApplyWorldEdits(create_info.world_edits);
		configured_world_edits_applied_ = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::World, "Applied configured public API world edits");
	}

	void EngineRuntime::ApplyWorldEdits(const std::vector<WorldBlockEdit>& edits)
	{
		if (model_ == nullptr) return;

		ve::world::World& world = model_->MutableWorld();
		for (const WorldBlockEdit& edit : edits)
		{
			(void)ve::world::ApplyWorldBlockEdit(world, edit);
		}
	}
