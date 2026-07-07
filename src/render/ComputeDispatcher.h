#pragma once

#include <cstdint>

namespace ve::rendering
{
	/** Backend-neutral compute program identifier. */
	struct ComputeProgramHandle
	{
		std::uint32_t value = 0;

		[[nodiscard]] constexpr bool IsValid() const noexcept { return value != 0; }
	};

	/** Compute dispatch dimensions in work groups. */
	struct ComputeDispatchSize
	{
		std::uint32_t groups_x = 1;
		std::uint32_t groups_y = 1;
		std::uint32_t groups_z = 1;
	};

	/** Backend-neutral compute dispatch contract. */
	class ComputeDispatcher
	{
	public:
		virtual ~ComputeDispatcher() = default;
		[[nodiscard]] virtual bool IsSupported() const noexcept = 0;
		virtual void Dispatch(ComputeProgramHandle program, ComputeDispatchSize size) const = 0;
	};
}
