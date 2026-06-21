#include "RigidBody.h"

namespace ve::physics
{
	/** Accumulates a force for the next integration step. */
	void RigidBody::ApplyForce(const glm::vec3& force) noexcept
	{
		if (!is_static_) accumulated_force_ += force;
	}

	/** Applies an instant velocity change. */
	void RigidBody::ApplyImpulse(const glm::vec3& impulse) noexcept
	{
		if (!is_static_) velocity_ += impulse * inverse_mass_;
	}

	/** Clears accumulated forces after they are consumed. */
	void RigidBody::ClearForces() noexcept
	{
		accumulated_force_ = glm::vec3{ 0.0f };
	}

	/** Integrates force accumulation into linear velocity. */
	void RigidBody::IntegrateForces(const glm::vec3& gravity, float delta_seconds) noexcept
	{
		if (is_static_) return;
		const glm::vec3 acceleration = gravity + accumulated_force_ * inverse_mass_;
		velocity_ += acceleration * delta_seconds;
		ClearForces();
	}

	/** Integrates linear velocity into position. */
	void RigidBody::IntegrateVelocity(float delta_seconds) noexcept
	{
		if (!is_static_) position_ += velocity_ * delta_seconds;
	}
}
