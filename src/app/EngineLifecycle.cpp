#include "VoxelSandboxModule.h"

#include "Window.h"

#include <algorithm>

/// Updates projection matrices from resize events queued by the window backend.
void ve::engine::VoxelSandboxModule::UpdateProjectionIfWindowChanged()
{
	for (const WindowEvent& event : window_->DrainEvents())
	{
		if (event.kind == WindowEvent::Kind::FramebufferResized)
		{
			ApplyFramebufferSize(event.framebuffer_resized.width, event.framebuffer_resized.height);
		}
	}

	if (window_state_.current_width == 0 || window_state_.current_height == 0)
	{
		ApplyFramebufferSize(window_->GetWidth(), window_->GetHeight());
	}
}

/// Updates projection matrices only when the framebuffer dimensions changed.
void ve::engine::VoxelSandboxModule::ApplyFramebufferSize(int width, int height)
{
	const int clamped_width = std::max(1, width);
	const int clamped_height = std::max(1, height);
	if (clamped_width == window_state_.current_width && clamped_height == window_state_.current_height)
	{
		return;
	}
	window_state_.current_width = clamped_width;
	window_state_.current_height = clamped_height;
	UpdateProjections(window_state_.current_width, window_state_.current_height);
}
