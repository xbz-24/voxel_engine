							const std::size_t next_row_cell_index =
								ve::core::ToIndex((v_coordinate + height) * u_axis_block_count + u_coordinate + column);
							can_grow = CanMerge(merge_start_cell, mask[next_row_cell_index]);
							if (!can_grow) break;
						}
						if (can_grow) height++;
					}
					faces.push_back(BuildFace(
						axis_plan,
						merge_start_cell,
						normal_coordinate,
						u_coordinate,
						v_coordinate,
						width,
						height));
					if (diagnostics) diagnostics->merged_face_count++;
					ClearMergedCells(mask, u_axis_block_count, u_coordinate, v_coordinate, width, height);
					u_coordinate += width;
				}
			}
		}
	}
}
