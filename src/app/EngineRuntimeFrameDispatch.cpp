#include "EngineRuntime.h"

namespace ve::engine
{
	void EngineRuntime::RunFrame()
	{
		frame_timer_.Tick();
		RuntimeModuleContext context{ window_, asset_paths_, frame_timer_ };
		module_->RunFrame(context);
		EndRuntimeFrame();
	}
}
