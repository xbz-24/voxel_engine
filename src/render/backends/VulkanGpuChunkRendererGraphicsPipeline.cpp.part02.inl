		pipeline_info.pVertexInputState = &vertex_input;
		pipeline_info.pInputAssemblyState = &assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterization;
		pipeline_info.pMultisampleState = &multisample;
		pipeline_info.pDepthStencilState = &depth;
		pipeline_info.pColorBlendState = &blend;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.layout = pipeline_layout_;
		pipeline_info.renderPass = settings.render_pass;
		return vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1u, &pipeline_info, nullptr, &output_pipeline) == VK_SUCCESS;
	}
}
