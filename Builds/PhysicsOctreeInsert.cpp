#include "PhysicsOctree.h"

namespace ve::physics
{
	/// Inserts one proxy into a node or child node.
	void PhysicsOctree::InsertIntoNode(int node_index, const PhysicsProxy& proxy)
	{
		OctreeNode& node = nodes_[static_cast<ve::core::Index>(node_index)];
		if (node.depth >= max_depth_)
		{
			node.proxies.push_back(proxy);
			return;
		}
		const int child_slot = ChildSlotForPoint(node, proxy.bounds.Center());
		if (node.children[child_slot] < 0)
		{
			node.children[child_slot] = CreateNode(ChildBounds(node, child_slot), node.depth + 1);
		}
		InsertIntoNode(node.children[child_slot], proxy);
	}

	/// Selects a child slot from a point position.
	int PhysicsOctree::ChildSlotForPoint(const OctreeNode& node, const glm::vec3& point) const
	{
		const glm::vec3 center = node.bounds.Center();
		return (point.x >= center.x ? 1 : 0) |
			(point.y >= center.y ? 2 : 0) |
			(point.z >= center.z ? 4 : 0);
	}

	/// Builds one child bounds from its slot bits.
	Aabb PhysicsOctree::ChildBounds(const OctreeNode& node, int child_slot) const
	{
		const glm::vec3 center = node.bounds.Center();
		Aabb child = node.bounds;
		if ((child_slot & 1) != 0) child.minimum.x = center.x; else child.maximum.x = center.x;
		if ((child_slot & 2) != 0) child.minimum.y = center.y; else child.maximum.y = center.y;
		if ((child_slot & 4) != 0) child.minimum.z = center.z; else child.maximum.z = center.z;
		return child;
	}
}
