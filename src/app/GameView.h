#pragma once

#include "OpenGLRenderView.h"

namespace ve::engine
{
	// TODO: Delete this compatibility alias after callers depend on RenderView/OpenGLRenderView explicitly.
	/** Compatibility alias kept while old callers migrate to OpenGLRenderView. */
	using GameView = OpenGLRenderView;
}
