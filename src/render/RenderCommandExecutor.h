#pragma once

#include "RenderCommandList.h"

namespace ve::rendering
{
	/** Strategy interface used to execute backend-neutral render commands. */
	class RenderCommandExecutor
	{
	public:
		/** Releases backend command execution resources through concrete executors. */
		virtual ~RenderCommandExecutor() = default;

		/** @param command_list Commands to submit to the concrete graphics backend. */
		virtual void Execute(const RenderCommandList& command_list) = 0;
	};
}
