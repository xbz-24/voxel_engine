#include "EngineRuntimeEnvironment.h"

#include <cstdlib>
#include <cstring>
#include <memory>

namespace ve::engine::runtime_detail
{
	bool EnvironmentFlagEnabled(const char* name) noexcept
	{
#if defined(_MSC_VER)
		char* raw_value = nullptr;
		std::size_t value_size = 0;
		if (_dupenv_s(&raw_value, &value_size, name) != 0 || raw_value == nullptr) return false;
		std::unique_ptr<char, decltype(&std::free)> value(raw_value, std::free);
		const bool enabled = std::strcmp(value.get(), "1") == 0;
		return enabled;
#else
		const char* value = std::getenv(name);
		return value != nullptr && std::strcmp(value, "1") == 0;
#endif
	}
}
