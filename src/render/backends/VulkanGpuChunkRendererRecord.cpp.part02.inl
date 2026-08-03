		glm::mat4 projection = glm::perspectiveRH_ZO(
			glm::radians(72.0f),
			aspect,
			0.05f,
			FarWorldClipDistance);
		projection[1][1] *= -1.0f;
		const glm::mat4 world_to_view = camera.GetWorldToViewMatrix();
		const glm::mat4 sky_view = glm::mat4{ glm::mat3{ world_to_view } };
		const glm::mat4 inverse_sky_view_projection = glm::inverse(projection * sky_view);

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sky_pipeline_);
		PushVoxelShaderConstants(command_buffer, pipeline_layout_, inverse_sky_view_projection);
		vkCmdDraw(command_buffer, 3u, 1u, 0u, 0u);

		if (index_count_ > 0u)
		{
			vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, voxel_pipeline_);
			PushVoxelShaderConstants(command_buffer, pipeline_layout_, projection * world_to_view);
			const VkDeviceSize offset = 0u;
			vkCmdBindVertexBuffers(command_buffer, 0u, 1u, &vertex_buffer_, &offset);
			vkCmdBindIndexBuffer(command_buffer, index_buffer_, 0u, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(command_buffer, index_count_, 1u, 0u, 0, 0u);
		}
		if (overlay_callback != nullptr) overlay_callback(command_buffer, overlay_user_data);
		vkCmdEndRenderPass(command_buffer);
		return true;
	}
}
