namespace
{
#if !defined(NDEBUG)
	bool EnvironmentFlagEnabled(const char* name) noexcept
	{
#if defined(_MSC_VER)
		char* value = nullptr;
		std::size_t value_size = 0;
		if (_dupenv_s(&value, &value_size, name) != 0 || value == nullptr) return false;
		const bool enabled = std::strcmp(value, "1") == 0;
		std::free(value);
		return enabled;
#else
		const char* value = std::getenv(name);
		return value != nullptr && std::strcmp(value, "1") == 0;
#endif
	}
#endif
}
