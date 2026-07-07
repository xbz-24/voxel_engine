#include "PhysicsBvh.h"

#include <algorithm>
#include <cstddef>

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
		if (!proxies_.empty()) BuildNode(0, proxies_.size(), -1, 0);
	}

	/// Builds one recursive node over a proxy range.
	int PhysicsBvh::BuildNode(ve::core::Index begin, ve::core::Index end, int parent, int depth)
	{
		Aabb bounds = proxies_[begin].bounds;
		for (ve::core::Index index = begin + 1; index < end; index++) bounds = MergeBounds(bounds, proxies_[index].bounds);
		const int node_index = static_cast<int>(nodes_.size());
		const ve::core::Index node_slot = static_cast<ve::core::Index>(node_index);
		BvhNode node{};
		node.bounds = bounds;
		node.parent = parent;
		node.depth = depth;
		nodes_.push_back(node);
		if (end - begin == 1)
		{
			nodes_[node_slot].proxy_index = begin;
			nodes_[node_slot].is_leaf = true;
			return node_index;
		}
		const int axis = WidestAxis(bounds);
		const ve::core::Index middle = begin + ((end - begin) / 2);
		const auto begin_offset = static_cast<std::ptrdiff_t>(begin);
		const auto middle_offset = static_cast<std::ptrdiff_t>(middle);
		const auto end_offset = static_cast<std::ptrdiff_t>(end);
		std::nth_element(proxies_.begin() + begin_offset, proxies_.begin() + middle_offset, proxies_.begin() + end_offset,
			[axis](const PhysicsProxy& left, const PhysicsProxy& right)
			{
				return AxisValue(left.bounds.Center(), axis) < AxisValue(right.bounds.Center(), axis);
			});
		nodes_[node_slot].left_child = BuildNode(begin, middle, node_index, depth + 1);
		nodes_[node_slot].right_child = BuildNode(middle, end, node_index, depth + 1);
		return node_index;
	}

	/// Reports whether the tree has no nodes.
	bool PhysicsBvh::IsEmpty() const noexcept { return nodes_.empty(); }

	/// Reports the number of built BVH nodes.
	ve::core::Index PhysicsBvh::NodeCount() const noexcept { return nodes_.size(); }

	/// Updates one proxy and refits ancestor bounds without rebuilding tree topology.
	bool PhysicsBvh::UpdateProxyBounds(unsigned int proxy_id, const Aabb& bounds)
	{
		for (BvhNode& node : nodes_)
		{
			if (!node.is_leaf || proxies_[node.proxy_index].id != proxy_id) continue;
			proxies_[node.proxy_index].bounds = bounds;
			node.bounds = bounds;
			RefitAncestors(node.parent);
			return true;
		}
		return false;
	}

	/// Recomputes one internal node from its children.
	void PhysicsBvh::RefitNodeBounds(int node_index)
	{
		BvhNode& node = nodes_[static_cast<ve::core::Index>(node_index)];
		if (node.is_leaf)
		{
			node.bounds = proxies_[node.proxy_index].bounds;
			return;
		}

		const BvhNode& left_child = nodes_[static_cast<ve::core::Index>(node.left_child)];
		const BvhNode& right_child = nodes_[static_cast<ve::core::Index>(node.right_child)];
		node.bounds = MergeBounds(left_child.bounds, right_child.bounds);
	}

	/// Walks upward and refreshes parent bounds.
	void PhysicsBvh::RefitAncestors(int node_index)
	{
		while (node_index >= 0)
		{
			RefitNodeBounds(node_index);
			node_index = nodes_[static_cast<ve::core::Index>(node_index)].parent;
		}
	}

	/// Converts one internal node to stable debug metadata.
	PhysicsBvh::NodeDebugInfo PhysicsBvh::DebugNode(ve::core::Index node_index) const
	{
		const BvhNode& node = nodes_[node_index];
		NodeDebugInfo debug_info{};
		debug_info.bounds = node.bounds;
		debug_info.parent = node.parent;
		debug_info.depth = node.depth;
		debug_info.left_child = node.left_child;
		debug_info.right_child = node.right_child;
		debug_info.proxy_id = node.is_leaf ? proxies_[node.proxy_index].id : 0;
		debug_info.is_leaf = node.is_leaf;
		return debug_info;
	}
}
