#include "EngineRuntime.h"

namespace ve::engine
{
/** Renders editor panels and presents the native window. */
	void EngineRuntime::EndRuntimeFrame()
	{
		window_.Update();
	}
}
