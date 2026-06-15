#pragma once

#include <cstdint>

namespace ve::ecs
{
	struct Entity
	{
		std::uint32_t id = 0;
		std::uint32_t generation = 0;

		/** @return True when the handle points to a non-null entity id. */
		[[nodiscard]] bool IsValid() const noexcept;
	};

	/** @param left First entity. @param right Second entity. @return True when both handles match. */
	[[nodiscard]] bool operator==(Entity left, Entity right) noexcept;
}
