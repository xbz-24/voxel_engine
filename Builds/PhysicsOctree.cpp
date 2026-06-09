#include "PhysicsOctree.h"

namespace ve::physics
{
	/// Creates an octree over a fixed world-space root.
	PhysicsOctree::PhysicsOctree(Aabb root_bounds, int max_depth)
		: root_bounds_(root_bounds), max_depth_(max_depth)
	{
		Clear();
	}

	/// Removes every node and recreates the root.
	void PhysicsOctree::Clear()
	{
		nodes_.clear();
		CreateNode(root_bounds_, 0);
	}

	/// Creates one octree node.
	int PhysicsOctree::CreateNode(const Aabb& bounds, int depth)
	{
		const int node_index = static_cast<int>(nodes_.size());
		OctreeNode node{ bounds };
		node.children.fill(-1);
		node.depth = depth;
		nodes_.push_back(ve::core::Move(node));
		return node_index;
	}

	/// Inserts one proxy into the octree.
	void PhysicsOctree::Insert(const PhysicsProxy& proxy)
	{
		if (!nodes_.empty() && root_bounds_.Intersects(proxy.bounds)) InsertIntoNode(0, proxy);
	}

	/// Returns the number of allocated octree nodes.
	ve::core::Index PhysicsOctree::NodeCount() const noexcept { return nodes_.size(); }
}
