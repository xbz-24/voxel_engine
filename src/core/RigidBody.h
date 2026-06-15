#pragma once

#include <glm/glm.hpp>

namespace ve::physics
{
	/** Describes the starting state used when a rigid body is created. */
	struct RigidBodyCreateInfo
	{
		glm::vec3 position{ 0.0f };
		glm::vec3 velocity{ 0.0f };
		float mass = 1.0f;
		float restitution = 0.2f;
		bool is_static = false;
	};

	/** Simulates a single linear rigid body with mass, forces and velocity. */
	class RigidBody
	{
	public:
		/** @param create_info Initial transform, velocity and mass settings. */
		explicit RigidBody(const RigidBodyCreateInfo& create_info = {});

		/** @param mass New body mass in kilograms. */
		void SetMass(float mass) noexcept;

		/** @param is_static True when the body should not integrate movement. */
		void SetStatic(bool is_static) noexcept;

		/** @param position New world-space center of mass. */
		void SetPosition(const glm::vec3& position) noexcept;

		/** @param velocity New linear velocity in world units per second. */
		void SetVelocity(const glm::vec3& velocity) noexcept;

		/** @param restitution Bounce factor used by simple contact solvers. */
		void SetRestitution(float restitution) noexcept;

		/** @param force World-space force accumulated until the next physics step. */
		void ApplyForce(const glm::vec3& force) noexcept;

		/** @param impulse Instant velocity change scaled by inverse mass. */
		void ApplyImpulse(const glm::vec3& impulse) noexcept;

		/** Clears force accumulation after integration. */
		void ClearForces() noexcept;

		/** @param gravity Global acceleration. @param delta_seconds Step duration. */
		void IntegrateForces(const glm::vec3& gravity, float delta_seconds) noexcept;

		/** @param delta_seconds Step duration used to advance position. */
		void IntegrateVelocity(float delta_seconds) noexcept;

		/** @return World-space center of mass. */
		[[nodiscard]] const glm::vec3& Position() const noexcept;

		/** @return Linear velocity in world units per second. */
		[[nodiscard]] const glm::vec3& Velocity() const noexcept;

		/** @return Mass in kilograms. */
		[[nodiscard]] float Mass() const noexcept;

		/** @return Inverse mass, or zero for static bodies. */
		[[nodiscard]] float InverseMass() const noexcept;

		/** @return Bounce factor used by simple contact solvers. */
		[[nodiscard]] float Restitution() const noexcept;

		/** @return True when integration should skip this body. */
		[[nodiscard]] bool IsStatic() const noexcept;

	private:
		/** Updates inverse mass after mass or static state changes. */
		void RecalculateInverseMass() noexcept;

		glm::vec3 position_{ 0.0f };
		glm::vec3 velocity_{ 0.0f };
		glm::vec3 accumulated_force_{ 0.0f };
		float mass_ = 1.0f;
		float inverse_mass_ = 1.0f;
		float restitution_ = 0.2f;
		bool is_static_ = false;
	};
}
