#include "PhysicsBvh.h"

#include "PhysicsBvhBounds.h"

#include <algorithm>

namespace ve::physics
{
	void PhysicsBvh::Build(std::span<const PhysicsProxy> proxies)
	{
		proxies_.assign(proxies.begin(), proxies.end());
		nodes_.clear();
		if (!proxies_.empty()) BuildNode(0, proxies_.size(), -1, 0);
	}

	int PhysicsBvh::BuildNode(ve::core::Index begin, ve::core::Index end, int parent, int depth)
	{
		Aabb bounds = proxies_[begin].bounds;
		for (ve::core::Index index = begin + 1; index < end; index++)
		{
			bounds = detail::MergeBounds(bounds, proxies_[index].bounds);
		}

		const int nodeIndex = ve::core::ToInt(nodes_.size());
		const ve::core::Index nodeSlot = ve::core::ToIndex(nodeIndex);
		nodes_.push_back(BvhNode{ bounds, parent, depth });
		if (end - begin == 1)
		{
			nodes_[nodeSlot].proxy_index = begin;
			nodes_[nodeSlot].is_leaf = true;
			return nodeIndex;
		}

		const int axis = detail::WidestAxis(bounds);
		const ve::core::Index middle = begin + ((end - begin) / 2);
		std::nth_element(
			proxies_.begin() + ve::core::ToPtrdiff(begin),
			proxies_.begin() + ve::core::ToPtrdiff(middle),
			proxies_.begin() + ve::core::ToPtrdiff(end),
			[axis](const PhysicsProxy& left, const PhysicsProxy& right)
			{
				return detail::AxisValue(left.bounds.Center(), axis) <
					detail::AxisValue(right.bounds.Center(), axis);
			});
		nodes_[nodeSlot].left_child = BuildNode(begin, middle, nodeIndex, depth + 1);
		nodes_[nodeSlot].right_child = BuildNode(middle, end, nodeIndex, depth + 1);
		return nodeIndex;
	}
}
