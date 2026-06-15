#include "PhysicsOctree.h"

namespace ve::physics
{
	/// Finds all proxy ids whose bounds overlap the query box.
	void PhysicsOctree::QueryOverlaps(const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const
	{
		if (!nodes_.empty()) QueryNode(0, bounds, output_ids);
	}

	/// Queries one octree node recursively.
	void PhysicsOctree::QueryNode(int node_index, const Aabb& bounds, ve::core::DynamicArray<unsigned int>& output_ids) const
	{
		const OctreeNode& node = nodes_[static_cast<ve::core::Index>(node_index)];
		if (!node.bounds.Intersects(bounds)) return;
		for (const PhysicsProxy& proxy : node.proxies)
		{
			if (proxy.bounds.Intersects(bounds)) output_ids.push_back(proxy.id);
		}
		for (const int child_index : node.children)
		{
			if (child_index >= 0) QueryNode(child_index, bounds, output_ids);
		}
	}
}
