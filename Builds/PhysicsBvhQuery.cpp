#include "PhysicsBvh.h"

namespace ve::physics
{
	/// Finds all proxy ids whose bounds overlap the query box.
	void PhysicsBvh::QueryOverlaps(const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const
	{
		if (!nodes_.empty()) QueryNode(0, bounds, output_ids);
	}

	/// Queries one BVH node recursively.
	void PhysicsBvh::QueryNode(int node_index, const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const
	{
		const BvhNode& node = nodes_[static_cast<ve::core::Index>(node_index)];
		if (!node.bounds.Intersects(bounds)) return;
		if (node.is_leaf)
		{
			output_ids.push_back(proxies_[node.proxy_index].id);
			return;
		}
		QueryNode(node.left_child, bounds, output_ids);
		QueryNode(node.right_child, bounds, output_ids);
	}
}
