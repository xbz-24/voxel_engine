#include "PhysicsBvh.h"

#include "PhysicsBvhBounds.h"

namespace ve::physics
{
	bool PhysicsBvh::UpdateProxyBounds(unsigned int proxyId, const Aabb& bounds)
	{
		for (BvhNode& node : nodes_)
		{
			if (!node.is_leaf || proxies_[node.proxy_index].id != proxyId) continue;
			proxies_[node.proxy_index].bounds = bounds;
			node.bounds = bounds;
			RefitAncestors(node.parent);
			return true;
		}
		return false;
	}

	void PhysicsBvh::RefitNodeBounds(int nodeIndex)
	{
		BvhNode& node = nodes_[ve::core::ToIndex(nodeIndex)];
		if (node.is_leaf)
		{
			node.bounds = proxies_[node.proxy_index].bounds;
			return;
		}
		const BvhNode& leftChild = nodes_[ve::core::ToIndex(node.left_child)];
		const BvhNode& rightChild = nodes_[ve::core::ToIndex(node.right_child)];
		node.bounds = detail::MergeBounds(leftChild.bounds, rightChild.bounds);
	}

	void PhysicsBvh::RefitAncestors(int nodeIndex)
	{
		while (nodeIndex >= 0)
		{
			RefitNodeBounds(nodeIndex);
			nodeIndex = nodes_[ve::core::ToIndex(nodeIndex)].parent;
		}
	}
}
