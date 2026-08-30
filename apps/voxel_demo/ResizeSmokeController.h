#pragma once

#include <voxel/RuntimeFrame.h>

namespace voxel_demo
{
	class ResizeSmokeController final
	{
	public:
		ResizeSmokeController(int resize_frame, int frame_limit) noexcept;

		void OnFrame(voxel::FrameContext& frame) noexcept;
		[[nodiscard]] bool CompletedSuccessfully() const noexcept;

	private:
		int resize_frame_;
		int frame_limit_;
		int observed_frames_ = 0;
		bool resize_succeeded_ = false;
		bool reached_frame_limit_ = false;
	};
}
