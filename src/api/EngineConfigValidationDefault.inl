namespace voxel::detail
{
	const IEngineConfigValidator& DefaultEngineConfigValidator() noexcept
	{
		static const EngineConfigValidator validator{};
		return validator;
	}
}
