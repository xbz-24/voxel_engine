#include "RigidBodyWorld.h"

#include <algorithm>

namespace ve::physics
{
	namespace
	{
		/** @param body Body constrained against the debug ground plane. */
		void ResolveGroundPlaneContact(RigidBody& body) noexcept
		{
			if (body.Position().y >= 0.0f) return;
			glm::vec3 corrected_position = body.Position();
			glm::vec3 corrected_velocity = body.Velocity();
			corrected_position.y = 0.0f;
			if (corrected_velocity.y < 0.0f) corrected_velocity.y *= -body.Restitution();
			body.SetPosition(corrected_position);
			body.SetVelocity(corrected_velocity);
		}
	}

	/** Advances the physics world by a clamped number of substeps. */
	void RigidBodyWorld::Step(float delta_seconds, int substep_count)
	{
		if (delta_seconds <= 0.0f) return;
		const int safe_substep_count = std::clamp(substep_count, 1, 16);
		const float fixed_delta_seconds = delta_seconds / static_cast<float>(safe_substep_count);
		for (int substep_index = 0; substep_index < safe_substep_count; ++substep_index)
		{
			IntegrateSubstep(fixed_delta_seconds);
		}
	}

	/** Integrates every body once and applies a simple ground-plane constraint. */
	void RigidBodyWorld::IntegrateSubstep(float fixed_delta_seconds)
	{
		for (RigidBody& body : bodies_)
		{
			body.IntegrateForces(gravity_, fixed_delta_seconds);
			body.IntegrateVelocity(fixed_delta_seconds);
			ResolveGroundPlaneContact(body);
		}
	}
}
