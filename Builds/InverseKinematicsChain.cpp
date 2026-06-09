#include "InverseKinematicsChain.h"

#include <glm/geometric.hpp>

#include <numeric>

namespace ve::animation
{
	/** Copies joint positions and rebuilds cached segment lengths. */
	void InverseKinematicsChain::SetJointPositions(std::span<const glm::vec3> joint_positions)
	{
		joint_positions_.assign(joint_positions.begin(), joint_positions.end());
		RebuildSegmentLengths();
	}

	/** Returns mutable joint positions for solvers. */
	std::span<glm::vec3> InverseKinematicsChain::MutableJointPositions() noexcept
	{
		return joint_positions_;
	}

	/** Returns read-only joint positions. */
	std::span<const glm::vec3> InverseKinematicsChain::JointPositions() const noexcept
	{
		return joint_positions_;
	}

	/** Returns cached segment lengths. */
	std::span<const float> InverseKinematicsChain::SegmentLengths() const noexcept
	{
		return segment_lengths_;
	}

	/** Returns the number of joints. */
	ve::core::Index InverseKinematicsChain::JointCount() const noexcept { return joint_positions_.size(); }

	/** Returns the root position or zero for an empty chain. */
	glm::vec3 InverseKinematicsChain::RootPosition() const noexcept
	{
		return joint_positions_.empty() ? glm::vec3{ 0.0f } : joint_positions_.front();
	}

	/** Returns the end-effector position or zero for an empty chain. */
	glm::vec3 InverseKinematicsChain::EndEffectorPosition() const noexcept
	{
		return joint_positions_.empty() ? glm::vec3{ 0.0f } : joint_positions_.back();
	}

	/** Returns the total chain reach. */
	float InverseKinematicsChain::TotalLength() const noexcept
	{
		return std::accumulate(segment_lengths_.begin(), segment_lengths_.end(), 0.0f);
	}

	/** Recomputes segment lengths between adjacent joints. */
	void InverseKinematicsChain::RebuildSegmentLengths()
	{
		segment_lengths_.clear();
		for (ve::core::Index joint_index = 1; joint_index < joint_positions_.size(); ++joint_index)
		{
			segment_lengths_.push_back(glm::distance(joint_positions_[joint_index - 1], joint_positions_[joint_index]));
		}
	}
}
