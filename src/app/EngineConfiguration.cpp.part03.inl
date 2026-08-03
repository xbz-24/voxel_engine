		if (create_info.render_distance_chunks < 0)
		{
			issues.push_back("render_distance_chunks must be zero or greater");
		}
		if (create_info.has_custom_camera)
		{
			if (!IsFinite(create_info.camera_position))
			{
				issues.push_back("camera_position must contain finite values");
			}
			if (!IsFinite(create_info.camera_look_at))
			{
				issues.push_back("camera_look_at must contain finite values");
			}
		}
		for (const WorldBlockEdit& world_edit : create_info.world_edits)
		{
			if (!IsKnownWorldBlockEditKind(world_edit.kind))
			{
				issues.push_back("world_edits contains an unknown edit kind");
				break;
			}
		}
		return issues;
	}

	ValidatedEngineCreateInfo::ValidatedEngineCreateInfo(EngineCreateInfo create_info)
		: create_info_(std::move(create_info))
	{
	}

	const EngineCreateInfo& ValidatedEngineCreateInfo::Value() const noexcept
	{
		return create_info_;
	}

	std::optional<ValidatedEngineCreateInfo> MakeValidatedEngineCreateInfo(
		EngineCreateInfo create_info,
		std::vector<std::string>* issues)
	{
		std::vector<std::string> validation_issues = ValidateEngineCreateInfo(create_info);
		if (issues != nullptr)
		{
			*issues = validation_issues;
		}
		if (!validation_issues.empty())
		{
			return std::nullopt;
		}
		return ValidatedEngineCreateInfo(std::move(create_info));
	}
}
