#pragma once

#include "RenderCommandExecutor.h"

namespace ve::rendering
{
	/** OpenGL compatibility executor for backend-neutral render commands. */
	class OpenGLRenderCommandExecutor final : public RenderCommandExecutor
	{
	public:
		/** @param command_list Commands to submit through the fixed-function OpenGL path. */
		void Execute(const RenderCommandList& command_list) override;
	};
}
