#pragma once

#include "CoreTypes.h"
#include "PhysicsBounds.h"

#include <span>

namespace ve::physics
{
	// TODO: Support incremental refit/update so moving entities do not require full BVH rebuilds every frame.
	class PhysicsBvh
	{
	public:
		struct NodeDebugInfo
		{
			Aabb bounds{};
			int parent = -1;
			int depth = 0;
			int left_child = -1;
			int right_child = -1;
			unsigned int proxy_id = 0;
			bool is_leaf = false;
		};

		/** @param proxies Physics objects copied into the acceleration structure. */
		void Build(std::span<const PhysicsProxy> proxies);

		/** @param bounds Query box. @param output_ids Receives overlapping proxy ids. */
		void QueryOverlaps(const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const;

		/** @return True when no nodes were built. */
		[[nodiscard]] bool IsEmpty() const noexcept;

		/** @return Number of nodes built for debug visualization and metrics. */
		[[nodiscard]] ve::core::Index NodeCount() const noexcept;

		/** @param node_index Node to inspect. @return Read-only debug metadata. */
		[[nodiscard]] NodeDebugInfo DebugNode(ve::core::Index node_index) const;

	private:
		struct BvhNode
		{
			Aabb bounds{};
			int parent = -1;
			int depth = 0;
			int left_child = -1;
			int right_child = -1;
			ve::core::Index proxy_index = 0;
			bool is_leaf = false;
		};

		/** @param begin First proxy. @param end One-past-last proxy. @return Node index. */
		int BuildNode(ve::core::Index begin, ve::core::Index end, int parent, int depth);

		/** @param node_index Node to query. @param bounds Query box. @param output_ids Receives ids. */
		void QueryNode(int node_index, const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const;

		ve::core::DynamicArray<PhysicsProxy> proxies_;
		ve::core::DynamicArray<BvhNode> nodes_;
	};
}
