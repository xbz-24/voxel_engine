			traversal.traveled_distance = traversal.next_axis_distance.y;
			traversal.next_axis_distance.y += traversal.axis_distance_delta.y;
			traversal.surface_normal = { 0, -traversal.block_step.y, 0 };
		}
		else
		{
			traversal.current_block.z += traversal.block_step.z;
			traversal.traveled_distance = traversal.next_axis_distance.z;
			traversal.next_axis_distance.z += traversal.axis_distance_delta.z;
			traversal.surface_normal = { 0, 0, -traversal.block_step.z };
		}
	}
}
