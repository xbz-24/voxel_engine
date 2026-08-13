#pragma once

#include "EngineRuntimeBridge.h"

namespace voxel::detail::config_translation
{
	[[nodiscard]] ve::blocks::BlockId ToInternalBlock(Block block) noexcept;
	[[nodiscard]] Block ToPublicBlock(ve::blocks::BlockId block) noexcept;
	[[nodiscard]] ve::engine::WorldBlockEdit ToInternalWorldEdit(const WorldEdit& edit);
	[[nodiscard]] ve::world::TerrainGeneratorKind ToInternalTerrainGenerator(TerrainGenerator generator) noexcept;
	[[nodiscard]] ve::world::TerrainSpawnBiome ToInternalTerrainBiome(TerrainBiome biome) noexcept;
	[[nodiscard]] ve::world::TerrainBlockPalette ToInternalTerrainPalette(TerrainPalette palette) noexcept;
	[[nodiscard]] ve::world::TerrainGenerationSettings ToInternalTerrainGeneration(const WorldConfig& world) noexcept;
	[[nodiscard]] glm::vec3 ToInternalVec3(Vec3 value) noexcept;
	[[nodiscard]] Vec3 ToPublicVec3(glm::vec3 value) noexcept;
	[[nodiscard]] ve::rendering::VoxelRenderStyle ToInternalVoxelRenderStyle(
		const VoxelRenderStyle& style) noexcept;
	[[nodiscard]] BlockPosition ToPublicBlockPosition(glm::ivec3 position) noexcept;
	[[nodiscard]] ve::rendering::GraphicsApi ToInternalApi(GraphicsApi api) noexcept;
	[[nodiscard]] ve::log::Level ToInternalLogLevel(LogLevel level) noexcept;
	[[nodiscard]] ve::engine::RuntimeLogSettings ToInternalLogging(LogSettings logging);

	void ApplyWindowConfigurationToCreateInfo(
		ve::engine::EngineCreateInfo& create_info,
		const EngineConfig& config);
	void ApplyStartupRenderingConfigurationToCreateInfo(
		ve::engine::EngineCreateInfo& create_info,
		const EngineConfig& config);
	void ApplyCameraConfigurationToCreateInfo(
		ve::engine::EngineCreateInfo& create_info,
		const EngineConfig& config);
	void AppendAssetSearchRootsToCreateInfo(
		ve::engine::EngineCreateInfo& create_info,
		const AssetCatalog& assets);
	void AppendWorldEditsToCreateInfo(
		ve::engine::EngineCreateInfo& create_info,
		const WorldConfig& world);
	void ApplyWorldConfigurationToCreateInfo(
		ve::engine::EngineCreateInfo& create_info,
		const EngineConfig& config);

	[[nodiscard]] FrameContext BuildPublicFrameContextFromRuntimeFrame(
		const ve::engine::RuntimeFrameContext& runtime_frame);
	void AppendPublicFrameCommandsToRuntimeFrame(
		const FrameContext& public_frame,
		ve::engine::RuntimeFrameContext& runtime_frame);
	[[nodiscard]] Diagnostics BuildPublicDiagnosticsFromRuntimeDiagnostics(
		const ve::engine::RuntimeDiagnostics& runtime_diagnostics);
	void BindPublicUpdateCallback(
		ve::engine::EngineCreateInfo& create_info,
		const UpdateCallback& update_callback);
	void BindPublicDiagnosticsCallback(
		ve::engine::EngineCreateInfo& create_info,
		const DiagnosticsCallback& diagnostics_callback);

	[[nodiscard]] const IEngineConfigTranslator& TranslatorInstance() noexcept;
}
