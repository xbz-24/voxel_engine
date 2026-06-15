#pragma once

#include "InverseKinematicsChain.h"

namespace ve::animation
{
	/** Runtime tuning values for the FABRIK inverse-kinematics solver. */
	struct FabrikInverseKinematicsSettings
	{
		int max_iterations = 12;
		float tolerance = 0.025f;
	};

	/** Solves joint chains toward a target using the FABRIK IK algorithm. */
	class FabrikInverseKinematicsSolver
	{
	public:
		/** @param settings Iteration and tolerance controls for the solver. */
		explicit FabrikInverseKinematicsSolver(FabrikInverseKinematicsSettings settings = {});

		/** @param chain Chain modified in place. @param target Desired end-effector position. @return True when tolerance was reached. */
		[[nodiscard]] bool Solve(InverseKinematicsChain& chain, const glm::vec3& target) const;

	private:
		/** @param chain Chain to stretch toward an unreachable target. @param target Desired end-effector position. */
		void StretchUnreachableTarget(InverseKinematicsChain& chain, const glm::vec3& target) const;

		/** @param chain Reachable chain to solve. @param target Desired end-effector position. @return True when solved. */
		[[nodiscard]] bool SolveReachableTarget(InverseKinematicsChain& chain, const glm::vec3& target) const;

		/** @param joints Mutable joint positions. @param lengths Segment lengths. @param target Desired end-effector position. */
		void BackwardPass(std::span<glm::vec3> joints, std::span<const float> lengths, const glm::vec3& target) const;

		/** @param joints Mutable joint positions. @param lengths Segment lengths. @param root Original root position. */
		void ForwardPass(std::span<glm::vec3> joints, std::span<const float> lengths, const glm::vec3& root) const;

		FabrikInverseKinematicsSettings settings_{};
	};
}
