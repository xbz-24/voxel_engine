#include "FabrikInverseKinematicsSolver.h"

#include <glm/geometric.hpp>

namespace ve::animation
{
	namespace
	{
		/** @param vector Direction vector. @return Safe unit direction. */
		glm::vec3 NormalizeOrZero(const glm::vec3& vector) noexcept
		{
			const float length = glm::length(vector);
			return length > 0.0001f ? vector / length : glm::vec3{ 0.0f };
		}
	}

	/** Pulls joints from the end effector back toward the root. */
	void FabrikInverseKinematicsSolver::BackwardPass(std::span<glm::vec3> joints, std::span<const float> lengths, const glm::vec3& target) const
	{
		joints.back() = target;
		for (ve::core::Index joint_index = joints.size() - 1; joint_index > 0; --joint_index)
		{
			const glm::vec3 direction = NormalizeOrZero(joints[joint_index - 1] - joints[joint_index]);
			joints[joint_index - 1] = joints[joint_index] + direction * lengths[joint_index - 1];
		}
	}

	/** Pushes joints from the root forward while preserving segment lengths. */
	void FabrikInverseKinematicsSolver::ForwardPass(std::span<glm::vec3> joints, std::span<const float> lengths, const glm::vec3& root) const
	{
		joints.front() = root;
		for (ve::core::Index joint_index = 1; joint_index < joints.size(); ++joint_index)
		{
			const glm::vec3 direction = NormalizeOrZero(joints[joint_index] - joints[joint_index - 1]);
			joints[joint_index] = joints[joint_index - 1] + direction * lengths[joint_index - 1];
		}
	}
}
