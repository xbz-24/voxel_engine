#include "ResizeSmokeController.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace voxel_demo
{
	namespace
	{
#if defined(_WIN32)
		[[nodiscard]] HWND FindSandboxWindowForCurrentProcess() noexcept
		{
			constexpr wchar_t WindowTitle[] = L"Voxel Sandbox";
			for (HWND window = FindWindowW(nullptr, WindowTitle); window != nullptr;
				window = FindWindowExW(nullptr, window, nullptr, WindowTitle))
			{
				DWORD process_id = 0;
				if (GetWindowThreadProcessId(window, &process_id) != 0 &&
					process_id == GetCurrentProcessId()) return window;
			}
			return nullptr;
		}
#endif

		[[nodiscard]] bool TryIncreaseSandboxWindowSize() noexcept
		{
#if defined(_WIN32)
			constexpr int ResizeIncrementPixels = 64;
			const HWND window = FindSandboxWindowForCurrentProcess();
			if (window == nullptr) return false;

			RECT original{};
			if (!GetWindowRect(window, &original)) return false;
			const int original_width = original.right - original.left;
			const int original_height = original.bottom - original.top;
			if (!SetWindowPos(window, nullptr, 0, 0,
				original_width + ResizeIncrementPixels,
				original_height + ResizeIncrementPixels,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE)) return false;

			RECT resized{};
			return GetWindowRect(window, &resized) &&
				resized.right - resized.left > original_width &&
				resized.bottom - resized.top > original_height;
#else
			return false;
#endif
		}
	}

	ResizeSmokeController::ResizeSmokeController(int resize_frame, int frame_limit) noexcept
		: resize_frame_(resize_frame), frame_limit_(frame_limit)
	{
	}

	void ResizeSmokeController::OnFrame(voxel::FrameContext& frame) noexcept
	{
		++observed_frames_;
		if (observed_frames_ == resize_frame_)
			resize_succeeded_ = TryIncreaseSandboxWindowSize();
		if (observed_frames_ < frame_limit_) return;
		reached_frame_limit_ = true;
		frame.commands.RequestClose();
	}

	bool ResizeSmokeController::CompletedSuccessfully() const noexcept
	{
		return resize_succeeded_ && reached_frame_limit_;
	}
}
