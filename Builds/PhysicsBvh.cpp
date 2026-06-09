#include "PhysicsBvh.h"

#include <algorithm>

namespace
{
	/// Merges two boxes into one enclosing box.
	ve::physics::Aabb MergeBounds(const ve::physics::Aabb& left, const ve::physics::Aabb& right)
	{
		return { glm::min(left.minimum, right.minimum), glm::max(left.maximum, right.maximum) };
	}

	/// Chooses the widest axis for a BVH split.
	int WidestAxis(const ve::physics::Aabb& bounds)
	{
		const glm::vec3 size = bounds.Size();
		if (size.x >= size.y && size.x >= size.z) return 0;
		return size.y >= size.z ? 1 : 2;
	}

	/// Reads a point component by axis index.
	float AxisValue(const glm::vec3& value, int axis)
	{
		if (axis == 0) return value.x;
		return axis == 1 ? value.y : value.z;
	}
}

namespace ve::physics
{
	/// Builds a binary BVH from physics proxies.
	void PhysicsBvh::Build(std::span<const PhysicsProxy> proxies)
	{
		proxies_.assign(proxies.begin(), proxies.end());
		nodes_.clear();
		if (!proxies_.empty()) BuildNode(0, proxies_.size());
	}

	/// Builds one recursive node over a proxy range.
	int PhysicsBvh::BuildNode(ve::core::Index begin, ve::core::Index end)
	{
		Aabb bounds = proxies_[begin].bounds;
		for (ve::core::Index index = begin + 1; index < end; index++) bounds = MergeBounds(bounds, proxies_[index].bounds);
		const int node_index = static_cast<int>(nodes_.size());
		nodes_.push_back(BvhNode{ bounds });
		if (end - begin == 1)
		{
			nodes_[node_index].proxy_index = begin;
			nodes_[node_index].is_leaf = true;
			return node_index;
		}
		const int axis = WidestAxis(bounds);
		const ve::core::Index middle = begin + ((end - begin) / 2);
		std::nth_element(proxies_.begin() + begin, proxies_.begin() + middle, proxies_.begin() + end,
			[axis](const PhysicsProxy& left, const PhysicsProxy& right)
			{
				return AxisValue(left.bounds.Center(), axis) < AxisValue(right.bounds.Center(), axis);
			});
		nodes_[node_index].left_child = BuildNode(begin, middle);
		nodes_[node_index].right_child = BuildNode(middle, end);
		return node_index;
	}

	/// Reports whether the tree has no nodes.
	bool PhysicsBvh::IsEmpty() const noexcept { return nodes_.empty(); }
}
