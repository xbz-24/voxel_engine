		}
		return false;
	}

	/// Recomputes one internal node from its children.
	void PhysicsBvh::RefitNodeBounds(int node_index)
	{
		BvhNode& node = nodes_[ve::core::ToIndex(node_index)];
		if (node.is_leaf)
		{
			node.bounds = proxies_[node.proxy_index].bounds;
			return;
		}

		const BvhNode& left_child = nodes_[ve::core::ToIndex(node.left_child)];
		const BvhNode& right_child = nodes_[ve::core::ToIndex(node.right_child)];
		node.bounds = MergeBounds(left_child.bounds, right_child.bounds);
	}

	/// Walks upward and refreshes parent bounds.
	void PhysicsBvh::RefitAncestors(int node_index)
	{
		while (node_index >= 0)
		{
			RefitNodeBounds(node_index);
			node_index = nodes_[ve::core::ToIndex(node_index)].parent;
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
