#include "EngineRuntimeBridge.h"

#include <algorithm>
#include <utility>

namespace voxel::detail
{
	namespace
	{
		[[nodiscard]] ve::blocks::BlockId ToInternalBlock(Block block) noexcept
		{
			switch (block)
			{
			case Block::Air: return ve::blocks::BlockId::Air;
			case Block::Grass: return ve::blocks::BlockId::Grass;
			case Block::Dirt: return ve::blocks::BlockId::Dirt;
			case Block::Stone: return ve::blocks::BlockId::Stone;
			case Block::Cobblestone: return ve::blocks::BlockId::Cobblestone;
			case Block::OakLog: return ve::blocks::BlockId::OakLog;
			case Block::OakPlanks: return ve::blocks::BlockId::OakPlanks;
			case Block::Sand: return ve::blocks::BlockId::Sand;
			case Block::Bricks: return ve::blocks::BlockId::Bricks;
			case Block::DiamondOre: return ve::blocks::BlockId::DiamondOre;
			case Block::CoalOre: return ve::blocks::BlockId::CoalOre;
			case Block::IronOre: return ve::blocks::BlockId::IronOre;
			case Block::Obsidian: return ve::blocks::BlockId::Obsidian;
			case Block::OakLeaves: return ve::blocks::BlockId::OakLeaves;
			case Block::Deepslate: return ve::blocks::BlockId::Deepslate;
			case Block::CopperOre: return ve::blocks::BlockId::CopperOre;
			case Block::GoldOre: return ve::blocks::BlockId::GoldOre;
			case Block::EmeraldOre: return ve::blocks::BlockId::EmeraldOre;
			case Block::LapisOre: return ve::blocks::BlockId::LapisOre;
			case Block::Gravel: return ve::blocks::BlockId::Gravel;
			case Block::MossBlock: return ve::blocks::BlockId::MossBlock;
			case Block::Snow: return ve::blocks::BlockId::Snow;
			case Block::MossyCobblestone: return ve::blocks::BlockId::MossyCobblestone;
			case Block::Granite: return ve::blocks::BlockId::Granite;
			case Block::Andesite: return ve::blocks::BlockId::Andesite;
			case Block::Diorite: return ve::blocks::BlockId::Diorite;
			case Block::Blackstone: return ve::blocks::BlockId::Blackstone;
			case Block::Basalt: return ve::blocks::BlockId::Basalt;
			case Block::Bedrock: return ve::blocks::BlockId::Bedrock;
			case Block::BirchLog: return ve::blocks::BlockId::BirchLog;
			case Block::BirchPlanks: return ve::blocks::BlockId::BirchPlanks;
			case Block::BirchLeaves: return ve::blocks::BlockId::BirchLeaves;
			case Block::SpruceLog: return ve::blocks::BlockId::SpruceLog;
			case Block::SprucePlanks: return ve::blocks::BlockId::SprucePlanks;
			case Block::CraftingTable: return ve::blocks::BlockId::CraftingTable;
			case Block::Bookshelf: return ve::blocks::BlockId::Bookshelf;
			case Block::Pumpkin: return ve::blocks::BlockId::Pumpkin;
			case Block::Melon: return ve::blocks::BlockId::Melon;
			case Block::HayBlock: return ve::blocks::BlockId::HayBlock;
			case Block::AmethystBlock: return ve::blocks::BlockId::AmethystBlock;
			case Block::Glass: return ve::blocks::BlockId::Glass;
			case Block::Water: return ve::blocks::BlockId::Water;
			case Block::SeaLantern: return ve::blocks::BlockId::SeaLantern;
			case Block::CherryLog: return ve::blocks::BlockId::CherryLog;
			case Block::CherryPlanks: return ve::blocks::BlockId::CherryPlanks;
			case Block::CherryLeaves: return ve::blocks::BlockId::CherryLeaves;
			case Block::Sandstone: return ve::blocks::BlockId::Sandstone;
			case Block::CutSandstone: return ve::blocks::BlockId::CutSandstone;
			case Block::RedSand: return ve::blocks::BlockId::RedSand;
			case Block::RedSandstone: return ve::blocks::BlockId::RedSandstone;
			case Block::Terracotta: return ve::blocks::BlockId::Terracotta;
			case Block::Cactus: return ve::blocks::BlockId::Cactus;
			case Block::DeadBush: return ve::blocks::BlockId::DeadBush;
			default: return ve::blocks::BlockId::Air;
			}
		}

		[[nodiscard]] ve::engine::WorldBlockEdit ToInternalWorldEdit(const WorldEdit& edit)
		{
			ve::engine::WorldBlockEdit result{};
			result.kind = edit.kind == WorldEdit::Kind::FillBox
				? ve::engine::WorldBlockEdit::Kind::FillBox
				: ve::engine::WorldBlockEdit::Kind::SetBlock;
			result.min_x = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.x : edit.position.x;
			result.min_y = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.y : edit.position.y;
			result.min_z = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.z : edit.position.z;
			result.max_x = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.x : edit.position.x;
			result.max_y = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.y : edit.position.y;
			result.max_z = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.z : edit.position.z;
			result.block = ToInternalBlock(edit.block);
			return result;
		}

		[[nodiscard]] glm::vec3 ToInternalVec3(Vec3 value) noexcept
		{
			return { value.x, value.y, value.z };
		}

		[[nodiscard]] ve::rendering::GraphicsApi ToInternalApi(GraphicsApi api) noexcept
		{
			switch (api)
			{
			case GraphicsApi::OpenGLCompatibility:
				return ve::rendering::GraphicsApi::OpenGLCompatibility;
			case GraphicsApi::DirectX12:
				return ve::rendering::GraphicsApi::DirectX12;
			case GraphicsApi::Vulkan:
			default:
				return ve::rendering::GraphicsApi::Vulkan;
			}
		}

		[[nodiscard]] ve::rendering::VulkanMinecraftDemoPreset ToInternalDemoPreset(DemoScene scene) noexcept
		{
			switch (scene)
			{
			case DemoScene::AquaModel:
				return ve::rendering::VulkanMinecraftDemoPreset::AquaModel;
			case DemoScene::SponzaAtrium:
				return ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium;
			case DemoScene::HyperrealDesert:
			default:
				return ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert;
			}
		}

		class EngineConfigTranslator final : public IEngineConfigTranslator
		{
		public:
			[[nodiscard]] ve::engine::EngineCreateInfo Translate(const EngineConfig& config) const override
			{
				ve::engine::EngineCreateInfo result{};
				result.window.title = config.window.title;
				result.window.width = std::max(1, config.window.width);
				result.window.height = std::max(1, config.window.height);
				result.window.fullscreen = config.window.fullscreen;
				result.render_backend.preferred_api = ToInternalApi(config.graphics_api);
				result.vulkan_demo_preset = ToInternalDemoPreset(config.demo_scene);
				result.camera_position = ToInternalVec3(config.camera.position);
				result.camera_look_at = ToInternalVec3(config.camera.look_at);
				result.has_custom_camera = config.camera.has_custom_view;
				result.vsync = config.window.vsync;
				result.show_debug_overlay = config.show_debug_overlay;
				result.world_size_chunks = std::max(1, config.world.size_chunks);
				result.render_distance_chunks = std::max(0, config.render_distance_chunks);
				result.world_edits.reserve(config.world.edits.size());
				for (const WorldEdit& edit : config.world.edits)
				{
					result.world_edits.push_back(ToInternalWorldEdit(edit));
				}
				if (config.on_update)
				{
					result.on_update = [callback = config.on_update](ve::engine::RuntimeFrameContext& frame) {
						FrameContext public_frame{};
						public_frame.delta_seconds = frame.delta_seconds;
						public_frame.elapsed_seconds = frame.elapsed_seconds;
						public_frame.fps = frame.fps;
						public_frame.input.move_forward = frame.input.move_forward;
						public_frame.input.move_left = frame.input.move_left;
						public_frame.input.move_back = frame.input.move_back;
						public_frame.input.move_right = frame.input.move_right;
						public_frame.input.jump = frame.input.jump;
						public_frame.input.escape = frame.input.escape;
						public_frame.input.f1 = frame.input.f1;
						public_frame.input.f2 = frame.input.f2;
						public_frame.input.primary_action = frame.input.primary_action;
						public_frame.input.mouse_x = frame.input.mouse_x;
						public_frame.input.mouse_y = frame.input.mouse_y;
						callback(public_frame);
						frame.world_edits.reserve(public_frame.commands.world_edits.size());
						for (const WorldEdit& edit : public_frame.commands.world_edits)
						{
							frame.world_edits.push_back(ToInternalWorldEdit(edit));
						}
						frame.request_close = public_frame.commands.request_close;
					};
				}
				if (config.on_diagnostics)
				{
					result.on_diagnostics = [callback = config.on_diagnostics](const ve::engine::RuntimeDiagnostics& diagnostics) {
						callback(Diagnostics{
							diagnostics.fps,
							diagnostics.pending_world_events,
							diagnostics.render_distance_chunks
						});
					};
				}
				result.on_log = config.on_log;
				return result;
			}
		};
	}

	const IEngineConfigTranslator& DefaultEngineConfigTranslator() noexcept
	{
		static const EngineConfigTranslator translator{};
		return translator;
	}
}
