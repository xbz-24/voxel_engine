#pragma once

#include "CoreTypes.h"
#include "RigidBody.h"

#include <cstdint>

namespace ve::physics
{
	/** Lightweight handle used to resolve a body inside RigidBodyWorld. */
	struct RigidBodyHandle
	{
		std::uint32_t index = UINT32_MAX;

		/** @return True when the handle points at a body slot. */
		[[nodiscard]] bool IsValid() const noexcept;
	};

	/** Owns rigid bodies and advances them with fixed substeps. */
	class RigidBodyWorld
	{
	public:
		/** @param gravity Acceleration applied to dynamic bodies every step. */
		explicit RigidBodyWorld(glm::vec3 gravity = { 0.0f, -9.81f, 0.0f });

		/** @param create_info Initial body settings. @return Handle to the new body. */
		[[nodiscard]] RigidBodyHandle CreateBody(const RigidBodyCreateInfo& create_info);

		/** @param handle Body handle to resolve. @return Body pointer or null. */
		[[nodiscard]] RigidBody* FindBody(RigidBodyHandle handle) noexcept;

		/** @param handle Body handle to resolve. @return Body pointer or null. */
		[[nodiscard]] const RigidBody* FindBody(RigidBodyHandle handle) const noexcept;

		/** @param delta_seconds Time to simulate. @param substep_count Number of fixed substeps. */
		void Step(float delta_seconds, int substep_count = 1);

		/** Removes every simulated body. */
		void Clear();

		/** @param gravity Acceleration applied to dynamic bodies. */
		void SetGravity(const glm::vec3& gravity) noexcept;

		/** @return Current world gravity. */
		[[nodiscard]] const glm::vec3& Gravity() const noexcept;

		/** @return Number of bodies stored by the world. */
		[[nodiscard]] ve::core::Index BodyCount() const noexcept;

	private:
		/** @param fixed_delta_seconds Duration of one physics substep. */
		void IntegrateSubstep(float fixed_delta_seconds);

		glm::vec3 gravity_{ 0.0f, -9.81f, 0.0f };
		ve::core::DynamicArray<RigidBody> bodies_;
	};
}
