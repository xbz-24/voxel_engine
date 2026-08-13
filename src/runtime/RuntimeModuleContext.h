#pragma once

namespace ve::assets { struct AssetPaths; }
namespace ve::time { class FrameTimer; }

namespace ve::engine
{
	class Window;

	/** Generic services supplied by the runtime host to its active content module. */
	struct RuntimeModuleContext
	{
		RuntimeModuleContext(Window& window,
			const ve::assets::AssetPaths& assets,
			const ve::time::FrameTimer& frame_timer) noexcept
			: window(window), assets(assets), frame_timer(frame_timer) {}

		Window& window;
		const ve::assets::AssetPaths& assets;
		const ve::time::FrameTimer& frame_timer;
	};
}
