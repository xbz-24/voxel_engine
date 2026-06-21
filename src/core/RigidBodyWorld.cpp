#include "RigidBodyWorld.h"

namespace ve::physics
{
	/** Reports whether a handle points at a possible body slot. */
	bool RigidBodyHandle::IsValid() const noexcept { return index != UINT32_MAX; }

	/** Creates a body world with a configurable gravity vector. */
	RigidBodyWorld::RigidBodyWorld(glm::vec3 gravity)
		: gravity_(gravity)
	{
	}

	/** Adds a body and returns its stable slot handle. */
	RigidBodyHandle RigidBodyWorld::CreateBody(const RigidBodyCreateInfo& create_info)
	{
		bodies_.emplace_back(create_info);
		return RigidBodyHandle{ static_cast<std::uint32_t>(bodies_.size() - 1) };
	}

	/** Resolves a mutable body handle. */
	RigidBody* RigidBodyWorld::FindBody(RigidBodyHandle handle) noexcept
	{
		if (!handle.IsValid() || handle.index >= bodies_.size()) return nullptr;
		return &bodies_[handle.index];
	}

	/** Resolves a read-only body handle. */
	const RigidBody* RigidBodyWorld::FindBody(RigidBodyHandle handle) const noexcept
	{
		if (!handle.IsValid() || handle.index >= bodies_.size()) return nullptr;
		return &bodies_[handle.index];
	}

	/** Removes every body from the world. */
	void RigidBodyWorld::Clear()
	{
		bodies_.clear();
	}

	/** Sets the world gravity vector. */
	void RigidBodyWorld::SetGravity(const glm::vec3& gravity) noexcept { gravity_ = gravity; }

	/** Returns the world gravity vector. */
	const glm::vec3& RigidBodyWorld::Gravity() const noexcept { return gravity_; }

	/** Returns the number of simulated bodies. */
	ve::core::Index RigidBodyWorld::BodyCount() const noexcept { return bodies_.size(); }
}
