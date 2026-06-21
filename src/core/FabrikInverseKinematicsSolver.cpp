#include "FabrikInverseKinematicsSolver.h"

#include <glm/geometric.hpp>

#include <algorithm>

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

	/** Stores clamped FABRIK solver settings. */
	FabrikInverseKinematicsSolver::FabrikInverseKinematicsSolver(FabrikInverseKinematicsSettings settings)
		: settings_{ std::max(settings.max_iterations, 1), std::max(settings.tolerance, 0.0001f) }
	{
	}

	/** Solves a chain toward the target using the FABRIK algorithm. */
	bool FabrikInverseKinematicsSolver::Solve(InverseKinematicsChain& chain, const glm::vec3& target) const
	{
		if (chain.JointCount() < 2) return false;
		const float distance_to_target = glm::distance(chain.RootPosition(), target);
		if (distance_to_target > chain.TotalLength())
		{
			StretchUnreachableTarget(chain, target);
			return false;
		}
		return SolveReachableTarget(chain, target);
	}

	/** Stretches all chain segments toward an unreachable target. */
	void FabrikInverseKinematicsSolver::StretchUnreachableTarget(InverseKinematicsChain& chain, const glm::vec3& target) const
	{
		std::span<glm::vec3> joints = chain.MutableJointPositions();
		std::span<const float> lengths = chain.SegmentLengths();
		for (ve::core::Index segment_index = 0; segment_index < lengths.size(); ++segment_index)
		{
			const glm::vec3 direction = NormalizeOrZero(target - joints[segment_index]);
			joints[segment_index + 1] = joints[segment_index] + direction * lengths[segment_index];
		}
	}

	/** Solves a reachable target with iterative backward and forward passes. */
	bool FabrikInverseKinematicsSolver::SolveReachableTarget(InverseKinematicsChain& chain, const glm::vec3& target) const
	{
		const glm::vec3 root = chain.RootPosition();
		std::span<glm::vec3> joints = chain.MutableJointPositions();
		std::span<const float> lengths = chain.SegmentLengths();
		for (int iteration_index = 0; iteration_index < settings_.max_iterations; ++iteration_index)
		{
			BackwardPass(joints, lengths, target);
			ForwardPass(joints, lengths, root);
			if (glm::distance(chain.EndEffectorPosition(), target) <= settings_.tolerance) return true;
		}
		return glm::distance(chain.EndEffectorPosition(), target) <= settings_.tolerance;
	}
}
