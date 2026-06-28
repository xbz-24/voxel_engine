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
		/** @param proxies Physics objects copied into the acceleration structure. */
		void Build(std::span<const PhysicsProxy> proxies);

		/** @param bounds Query box. @param output_ids Receives overlapping proxy ids. */
		void QueryOverlaps(const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const;

		/** @return True when no nodes were built. */
		[[nodiscard]] bool IsEmpty() const noexcept;

	private:
		struct BvhNode
		{
			// TODO: Store parent/depth metadata for debug visualization and tree quality metrics.
			Aabb bounds{};
			int left_child = -1;
			int right_child = -1;
			ve::core::Index proxy_index = 0;
			bool is_leaf = false;
		};

		/** @param begin First proxy. @param end One-past-last proxy. @return Node index. */
		int BuildNode(ve::core::Index begin, ve::core::Index end);

		/** @param node_index Node to query. @param bounds Query box. @param output_ids Receives ids. */
		void QueryNode(int node_index, const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const;

		ve::core::DynamicArray<PhysicsProxy> proxies_;
		ve::core::DynamicArray<BvhNode> nodes_;
	};
}
