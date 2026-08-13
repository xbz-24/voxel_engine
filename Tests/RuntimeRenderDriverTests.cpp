#include <doctest/doctest.h>

#include "RuntimeRenderDriver.h"
#include "RuntimeRenderDriverFactory.h"

#include <memory>

TEST_CASE("runtime render driver factory exposes backend texture policy without GPU startup")
{
	std::unique_ptr<ve::engine::RuntimeRenderDriver> opengl =
		ve::engine::RuntimeRenderDriverFactory::Create(
			ve::rendering::GraphicsApi::OpenGLCompatibility);
	std::unique_ptr<ve::engine::RuntimeRenderDriver> vulkan =
		ve::engine::RuntimeRenderDriverFactory::Create(
			ve::rendering::GraphicsApi::Vulkan);

	REQUIRE(opengl != nullptr);
	REQUIRE(vulkan != nullptr);
	CHECK(opengl->TextureLoading() ==
		ve::blocks::BlockRegistry::TextureLoading::LoadTextures);
	CHECK(vulkan->TextureLoading() ==
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	CHECK(!opengl->Capabilities().is_available);
	CHECK(!vulkan->Capabilities().is_available);
	opengl->Shutdown();
	opengl->Shutdown();
	vulkan->Shutdown();
	vulkan->Shutdown();
}

TEST_CASE("runtime render driver factory preserves unsupported and unknown failures")
{
	CHECK(ve::engine::RuntimeRenderDriverFactory::Create(
		ve::rendering::GraphicsApi::DirectX12) == nullptr);
	const ve::engine::EngineStartupResult unsupported =
		ve::engine::RuntimeRenderDriverFactory::MissingDriverResult(
			ve::rendering::GraphicsApi::DirectX12);
	CHECK(unsupported.failure ==
		ve::engine::EngineStartupFailure::UnsupportedRenderBackend);

	const auto unknown_api = static_cast<ve::rendering::GraphicsApi>(999);
	CHECK(ve::engine::RuntimeRenderDriverFactory::Create(unknown_api) == nullptr);
	const ve::engine::EngineStartupResult unavailable =
		ve::engine::RuntimeRenderDriverFactory::MissingDriverResult(unknown_api);
	CHECK(unavailable.failure ==
		ve::engine::EngineStartupFailure::RenderBackendUnavailable);
}
