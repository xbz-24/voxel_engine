#include "RuntimeRenderDriverFactory.h"

#include "OpenGLRuntimeRenderDriver.h"
#include "VulkanRuntimeRenderDriver.h"

namespace ve::engine
{
	std::unique_ptr<RuntimeRenderDriver> RuntimeRenderDriverFactory::Create(
		ve::rendering::GraphicsApi api)
	{
		switch (api)
		{
		case ve::rendering::GraphicsApi::OpenGLCompatibility:
			return std::make_unique<OpenGLRuntimeRenderDriver>();
		case ve::rendering::GraphicsApi::Vulkan:
			return std::make_unique<VulkanRuntimeRenderDriver>();
		case ve::rendering::GraphicsApi::DirectX12:
			return nullptr;
		}
		return nullptr;
	}

	EngineStartupResult RuntimeRenderDriverFactory::MissingDriverResult(
		ve::rendering::GraphicsApi api)
	{
		if (api == ve::rendering::GraphicsApi::DirectX12)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::UnsupportedRenderBackend,
				"Selected render backend is not implemented");
		}
		return EngineStartupResult::Failure(
			EngineStartupFailure::RenderBackendUnavailable,
			"Runtime render driver factory returned no driver");
	}
}
