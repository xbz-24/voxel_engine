#pragma once

#include "CoreTypes.h"
#include "PhysicsBounds.h"

namespace ve::physics
{
	class PhysicsOctree
	{
	public:
		/** @param root_bounds World-space box covered by the tree. @param max_depth Deepest child level. */
		PhysicsOctree(Aabb root_bounds, int max_depth);

		/** Removes every node and recreates the root. */
		void Clear();

		/** @param proxy Physics object copied into the octree. */
		void Insert(const PhysicsProxy& proxy);

		/** @param bounds Query box. @param output_ids Receives overlapping proxy ids. */
		void QueryOverlaps(const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const;

		/** @return Number of allocated octree nodes. */
		[[nodiscard]] ve::core::Index NodeCount() const noexcept;

	private:
		struct OctreeNode
		{
			Aabb bounds{};
			ve::core::StaticArray<int, 8> children{};
			ve::core::DynamicArray<PhysicsProxy> proxies{};
			int depth = 0;
		};

		/** @param bounds Node bounds. @param depth Node depth. @return Created node index. */
		int CreateNode(const Aabb& bounds, int depth);

		/** @param node_index Node receiving the proxy. @param proxy Object to insert. */
		void InsertIntoNode(int node_index, const PhysicsProxy& proxy);

		/** @param node Node to inspect. @param point Proxy center. @return Child index in [0,7]. */
		int ChildSlotForPoint(const OctreeNode& node, const glm::vec3& point) const;

		/** @param node Parent node. @param child_slot Child slot in [0,7]. @return Child bounds. */
		Aabb ChildBounds(const OctreeNode& node, int child_slot) const;

		/** @param node_index Node to query. @param bounds Query box. @param output_ids Receives ids. */
		void QueryNode(int node_index, const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const;

		Aabb root_bounds_{};
		int max_depth_ = 0;
		ve::core::DynamicArray<OctreeNode> nodes_;
	};
}
