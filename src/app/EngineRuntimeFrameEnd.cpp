#include "EngineRuntime.h"

#include "CoreTypes.h"
#include "OpenGLRenderView.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace ve::engine
{
/** Renders editor panels and presents the native window. */
	void EngineRuntime::EndRuntimeFrame()
	{
		if (view_->Api() == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			editor_controller_.DrawAndApply(window_, engine_.MutableRuntimeSettings());
			editor_controller_.Render();
		}
		window_.Update();
	}
}
