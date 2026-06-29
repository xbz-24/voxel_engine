#include "EngineRuntime.h"

#include "Logger.h"
#include "OpenGLRenderView.h"
#include "RenderBackendFactory.h"
#include "RenderViewFactory.h"
#include "VulkanBackend.h"

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <glm/glm.hpp>


#include "EngineRuntimeEnvironment.inl"
#include "EngineRuntimeLifecycle.inl"
#include "EngineRuntimeSystems.inl"
#include "EngineRuntimeWorldEdits.inl"
#include "EngineRuntimeShutdown.inl"
#include "EngineRuntimeVulkanInput.inl"
