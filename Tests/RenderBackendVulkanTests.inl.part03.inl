	malformed_texture.width = 0U;
	malformed_texture.height = 1U;
	CHECK(malformed_texture.Sample(0.5f, 0.5f) == ve::rendering::PackRgb({ 132, 132, 132 }));

	ve::rendering::VulkanRasterCpuTexture valid_texture;
	valid_texture.pixels = { 0x00123456U };
	valid_texture.width = 1U;
	valid_texture.height = 1U;
	CHECK(valid_texture.Sample(0.5f, 0.5f) == 0x00123456U);
}
