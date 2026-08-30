#include "PhysicsBvh.h"

namespace ve::physics
{
	bool PhysicsBvh::IsEmpty() const noexcept
	{
		return nodes_.empty();
	}

	ve::core::Index PhysicsBvh::NodeCount() const noexcept
	{
		return nodes_.size();
	}

	PhysicsBvh::NodeDebugInfo PhysicsBvh::DebugNode(ve::core::Index nodeIndex) const
	{
		const BvhNode& node = nodes_[nodeIndex];
		NodeDebugInfo debugInfo{};
		debugInfo.bounds = node.bounds;
		debugInfo.parent = node.parent;
		debugInfo.depth = node.depth;
		debugInfo.left_child = node.left_child;
		debugInfo.right_child = node.right_child;
		debugInfo.proxy_id = node.is_leaf ? proxies_[node.proxy_index].id : 0;
		debugInfo.is_leaf = node.is_leaf;
		return debugInfo;
	}
}
