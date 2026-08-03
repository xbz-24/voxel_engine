
#include "CoreTypes.h"
#include "VulkanGpuChunkRenderer.h"
#include "VulkanImGuiOverlay.h"
#include "VulkanRendererOverlay.h"
#include "VulkanSoftwareVoxelRasterizer.h"
#include "VulkanUploadBuffer.h"

#include <volk.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

class Camera;
namespace ve::engine { class Window; }

namespace ve::world
{
	class World;
}

namespace ve::blocks
{
	class BlockRegistry;
}

namespace ve::rendering
{
	class VulkanBackend;

	/** Presents Vulkan frames by copying a rasterized frame image into the swapchain. */
	class VulkanFrameOrchestrator
	{
	public:
		/** Releases Vulkan frame resources on destruction. */
		~VulkanFrameOrchestrator();

		/** @param backend Initialized Vulkan backend. @return True when frame resources are ready. */
		[[nodiscard]] bool Initialize(VulkanBackend& backend,
			ve::engine::Window& window,
			const std::filesystem::path& block_texture_directory,
			const VoxelRenderStyle& render_style,
			bool enable_imgui_overlay = true);

		/** @return True when the voxel world was copied to the swapchain and presented. */
		[[nodiscard]] bool DrawFrame(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			const VulkanFrameInput& input,
			VulkanOverlaySettings& overlay_settings);

		/** Waits for the device and releases frame resources. */
		void Release();

		/** @return True when the overlay wants exclusive mouse input for the current frame. */
		[[nodiscard]] bool WantsMouseInput() const noexcept;

		/** @return True when the overlay wants exclusive keyboard input for the current frame. */
		[[nodiscard]] bool WantsKeyboardInput() const noexcept;

	private:
		struct VulkanGpuFrameControls;

		/** Creates command pools and per-frame command buffers. */
		[[nodiscard]] bool CreateCommandResources();

		/** Creates fences and semaphores for frames in flight. */
		[[nodiscard]] bool CreateSynchronization();

		/** Creates timestamp query resources used by frame diagnostics. */
		[[nodiscard]] bool CreateTimestampQueries();

		/** Ensures the software framebuffer matches the swapchain extent. */
		[[nodiscard]] bool EnsureFrameBuffer(VkExtent2D extent);

		/** Ensures intermediate copy images match the swapchain extent and format. */
		[[nodiscard]] bool EnsureIntermediateImages(VkExtent2D extent, VkFormat format);

		/** Uploads the current software rasterizer frame into the frame upload buffer. */
		[[nodiscard]] bool UploadFramePixels(VulkanFrameTiming& timing, std::size_t frame_index);

		/** Records the software-rasterized copy and present command stream. */
		[[nodiscard]] bool RecordSoftwareCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index);
