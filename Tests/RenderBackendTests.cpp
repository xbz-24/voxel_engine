#include <doctest/doctest.h>

#include "DirectX12Backend.h"
#include "OpenGLCompatibilityBackend.h"
#include "RenderBackendCatalog.h"
#include "RenderBackendFactory.h"
#include "RenderBackendSelector.h"
#include "TextureLoader.h"
#include "VulkanBackendSettings.h"
#include "VulkanChunkMeshTranslator.h"
#include "VulkanDebugLabels.h"
#include "VulkanGpuChunkRendererTypes.h"
#include "VulkanMinecraftDemoProfiles.h"
#include "VulkanMinecraftDemoSettings.h"
#include "VulkanRenderView.h"
#include "VulkanSoftwareRasterizerColor.h"
#include "VulkanSoftwareVoxelRasterizerData.h"
#include "VulkanSwapchainChoices.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "RenderBackendSelectionTests.inl"
#include "RenderBackendCapabilityTests.inl"
#include "RenderBackendVulkanTests.inl"

TEST_CASE("decoded image validates rgba8 metadata and payload size")
{
	ve::rendering::DecodedImage image;
	image.width = 2;
	image.height = 1;
	image.source_channel_count = 3;
	image.rgba = {
		std::uint8_t{ 255 },
		std::uint8_t{ 0 },
		std::uint8_t{ 0 },
		std::uint8_t{ 255 },
		std::uint8_t{ 0 },
		std::uint8_t{ 255 },
		std::uint8_t{ 0 },
		std::uint8_t{ 255 }
	};

	CHECK(image.pixel_format == ve::rendering::ImagePixelFormat::Rgba8);
	CHECK(image.color_space == ve::rendering::TextureColorSpace::Srgb);
	CHECK(image.mip_level_count == 1);
	CHECK(image.IsValid());

	ve::rendering::DirectX12Backend backend;
	CHECK(ve::rendering::UploadTexture(backend, image) == nullptr);

	image.rgba.pop_back();
	CHECK(!image.IsValid());
	CHECK(ve::rendering::UploadTexture(backend, image) == nullptr);
}

TEST_CASE("vulkan debug labels are safe before debug utils are available")
{
	ve::rendering::VulkanDebugLabels labels;

	CHECK(!labels.IsAvailable());
	CHECK(!labels.NameObject(VK_OBJECT_TYPE_DEVICE, 1U, "device"));
	labels.Initialize(VK_NULL_HANDLE, true);
	CHECK(!labels.IsAvailable());
	labels.Release();
	CHECK(!labels.IsAvailable());
}
