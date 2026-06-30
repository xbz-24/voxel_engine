#pragma once

#include "RenderApi.h"
#include "RenderDistanceSettings.h"
#include "Window.h"
#include "Block.h"
#include "VulkanMinecraftDemoSettings.h"
#include "WorldConfiguration.h"

#include <glm/glm.hpp>

#include <functional>
#include <string>
#include <vector>

namespace ve::engine
{
	struct WorldBlockEdit
	{
		// TODO: Replace this internal mirror with a shared command translator so public and network edits cannot drift.
		enum class Kind
		{
			SetBlock,
			FillBox
		};

		Kind kind = Kind::SetBlock;
		int min_x = 0;
		int min_y = 0;
		int min_z = 0;
		int max_x = 0;
		int max_y = 0;
		int max_z = 0;
		ve::blocks::BlockId block = ve::blocks::BlockId::Air;
	};

	struct RuntimeInputSnapshot
	{
		// TODO: Carry action states from the central input system instead of sampling a small hard-coded subset.
		bool move_forward = false;
		bool move_left = false;
		bool move_back = false;
		bool move_right = false;
		bool jump = false;
		bool escape = false;
		bool f1 = false;
		bool f2 = false;
		bool primary_action = false;
		double mouse_x = 0.0;
		double mouse_y = 0.0;
	};

	struct RuntimeFrameContext
	{
		float delta_seconds = 0.0f;
		float elapsed_seconds = 0.0f;
		double fps = 0.0;
		RuntimeInputSnapshot input{};
		std::vector<WorldBlockEdit> world_edits;
		bool request_close = false;
	};

	struct RuntimeDiagnostics
	{
		double fps = 0.0;
		int pending_world_events = 0;
		int render_distance_chunks = 0;
		int pending_chunk_mesh_tasks = 0;
		int pending_chunk_mesh_uploads = 0;
		int pending_world_generation_tasks = 0;
	};

	struct EngineCreateInfo
	{
		// TODO: Add a validated builder layer so unchecked public config does not cross into runtime initialization.
		WindowCreateInfo window{};
		ve::rendering::RenderBackendConfiguration render_backend{};
		glm::vec3 camera_position{ 80.0f, 50.0f, 80.0f };
		glm::vec3 camera_look_at{ 80.0f, 50.0f, 79.0f };
		bool vsync = false;
		bool show_debug_overlay = true;
		bool has_custom_camera = false;
		ve::rendering::VulkanMinecraftDemoPreset vulkan_demo_preset = ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert;
		int world_size_chunks = 8;
		ve::world::TerrainGenerationSettings terrain_generation{};
		int render_distance_chunks = ve::gameplay::DefaultRenderDistanceChunks;
		std::vector<WorldBlockEdit> world_edits;
		std::function<void(RuntimeFrameContext&)> on_update;
		std::function<void(const RuntimeDiagnostics&)> on_diagnostics;
		std::function<void(const std::string&)> on_log;
	};
}
