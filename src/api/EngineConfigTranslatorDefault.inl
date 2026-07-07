	const IEngineConfigTranslator& DefaultEngineConfigTranslator() noexcept
	{
		static const EngineConfigTranslator translator{};
		return translator;
	}
}
