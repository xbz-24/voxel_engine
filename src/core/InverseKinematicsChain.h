#pragma once

#include "CoreTypes.h"

#include <glm/glm.hpp>

#include <span>

namespace ve::animation
{
	/** Stores a procedural animation chain from root joint to end effector. */
	class InverseKinematicsChain
	{
	public:
		/** @param joint_positions World-space joints from root to end effector. */
		void SetJointPositions(std::span<const glm::vec3> joint_positions);

		/** @return Mutable joint positions from root to end effector. */
		[[nodiscard]] std::span<glm::vec3> MutableJointPositions() noexcept;

		/** @return Read-only joint positions from root to end effector. */
		[[nodiscard]] std::span<const glm::vec3> JointPositions() const noexcept;

		/** @return Read-only segment lengths between adjacent joints. */
		[[nodiscard]] std::span<const float> SegmentLengths() const noexcept;

		/** @return Number of joints stored by the chain. */
		[[nodiscard]] ve::core::Index JointCount() const noexcept;

		/** @return World-space root position, or zero when empty. */
		[[nodiscard]] glm::vec3 RootPosition() const noexcept;

		/** @return World-space end-effector position, or zero when empty. */
		[[nodiscard]] glm::vec3 EndEffectorPosition() const noexcept;

		/** @return Total reachable length from root to end effector. */
		[[nodiscard]] float TotalLength() const noexcept;

	private:
		/** Rebuilds segment lengths from current joint positions. */
		void RebuildSegmentLengths();

		ve::core::DynamicArray<glm::vec3> joint_positions_;
		ve::core::DynamicArray<float> segment_lengths_;
	};
}
