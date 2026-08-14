#include "EngineRuntime.h"

namespace ve::engine
{
	void EngineRuntime::RunFrame()
	{
		window_.MakeGraphicsContextCurrent();
		frame_timer_.Tick();
		RuntimeModuleContext context{ window_, asset_paths_, frame_timer_ };
		module_lifecycle_.RunFrame(context);
		EndRuntimeFrame();
	}
}
