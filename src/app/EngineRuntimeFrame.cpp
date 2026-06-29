#include "EngineRuntime.h"

#include "OpenGLRenderView.h"

#include <algorithm>
#include <cassert>
#include <limits>


#include "EngineRuntimeFrameDispatch.inl"
#include "EngineRuntimeFrameVulkanInput.inl"
#include "EngineRuntimeFrameCallbacks.inl"
#include "EngineRuntimeFrameOpenGL.inl"
#include "EngineRuntimeFrameEnd.inl"
