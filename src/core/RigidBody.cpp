#include "RigidBody.h"

#include <algorithm>

namespace ve::physics
{
	/** Creates a rigid body from the provided initial settings. */
	RigidBody::RigidBody(const RigidBodyCreateInfo& create_info)
		: position_(create_info.position),
		velocity_(create_info.velocity),
		mass_(std::max(create_info.mass, 0.001f)),
		restitution_(create_info.restitution),
		is_static_(create_info.is_static)
	{
		RecalculateInverseMass();
	}

	/** Sets the body mass and updates inverse mass. */
	void RigidBody::SetMass(float mass) noexcept
	{
		mass_ = std::max(mass, 0.001f);
		RecalculateInverseMass();
	}

	/** Changes whether this body is static. */
	void RigidBody::SetStatic(bool is_static) noexcept
	{
		is_static_ = is_static;
		RecalculateInverseMass();
	}

	/** Sets the world-space body position. */
	void RigidBody::SetPosition(const glm::vec3& position) noexcept { position_ = position; }

	/** Sets the linear velocity. */
	void RigidBody::SetVelocity(const glm::vec3& velocity) noexcept { velocity_ = velocity; }

	/** Sets the bounce factor used by simple contact solvers. */
	void RigidBody::SetRestitution(float restitution) noexcept { restitution_ = std::clamp(restitution, 0.0f, 1.0f); }

	/** Returns the world-space center of mass. */
	const glm::vec3& RigidBody::Position() const noexcept { return position_; }

	/** Returns the linear velocity. */
	const glm::vec3& RigidBody::Velocity() const noexcept { return velocity_; }

	/** Returns the body mass. */
	float RigidBody::Mass() const noexcept { return mass_; }

	/** Returns inverse mass, or zero for static bodies. */
	float RigidBody::InverseMass() const noexcept { return inverse_mass_; }

	/** Returns the bounce factor. */
	float RigidBody::Restitution() const noexcept { return restitution_; }

	/** Reports whether this body skips integration. */
	bool RigidBody::IsStatic() const noexcept { return is_static_; }

	/** Updates inverse mass from current mass and static state. */
	void RigidBody::RecalculateInverseMass() noexcept
	{
		inverse_mass_ = is_static_ ? 0.0f : 1.0f / mass_;
	}
}
