#include <doctest/doctest.h>

#include "DirectX12Backend.h"
#include "OpenGLCompatibilityBackend.h"
#include "RenderBackendCatalog.h"
#include "RenderBackendFactory.h"
#include "RenderBackendSelector.h"
#include "VulkanChunkMeshTranslator.h"
#include "VulkanRenderView.h"
#include "VulkanSwapchainChoices.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "RenderBackendSelectionTests.inl"
#include "RenderBackendCapabilityTests.inl"
#include "RenderBackendVulkanTests.inl"
