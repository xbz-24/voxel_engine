#include "RenderBackendFactory.h"

#include "DirectX12Backend.h"
#include "OpenGLCompatibilityBackend.h"
#include "VulkanBackend.h"

namespace ve::rendering
{
	/** Creates an object for the requested graphics API. */
	std::unique_ptr<RenderBackend> RenderBackendFactory::Create(GraphicsApi api)
	{
		switch (api)
		{
		case GraphicsApi::Vulkan: return std::make_unique<VulkanBackend>();
		case GraphicsApi::OpenGLCompatibility: return std::make_unique<OpenGLCompatibilityBackend>();
		case GraphicsApi::DirectX12: return std::make_unique<DirectX12Backend>();
		}
		return nullptr;
	}
}
