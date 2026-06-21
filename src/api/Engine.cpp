#include "voxel/Engine.h"

#include "Engine.h"

#include <algorithm>
#include <utility>

namespace
{
	[[nodiscard]] ve::blocks::BlockId ToInternalBlock(voxel::Block block) noexcept
	{
		switch (block)
		{
		case voxel::Block::Air: return ve::blocks::BlockId::Air;
		case voxel::Block::Grass: return ve::blocks::BlockId::Grass;
		case voxel::Block::Dirt: return ve::blocks::BlockId::Dirt;
		case voxel::Block::Stone: return ve::blocks::BlockId::Stone;
		case voxel::Block::Cobblestone: return ve::blocks::BlockId::Cobblestone;
		case voxel::Block::OakLog: return ve::blocks::BlockId::OakLog;
		case voxel::Block::OakPlanks: return ve::blocks::BlockId::OakPlanks;
		case voxel::Block::Sand: return ve::blocks::BlockId::Sand;
		case voxel::Block::Bricks: return ve::blocks::BlockId::Bricks;
		case voxel::Block::DiamondOre: return ve::blocks::BlockId::DiamondOre;
		case voxel::Block::CoalOre: return ve::blocks::BlockId::CoalOre;
		case voxel::Block::IronOre: return ve::blocks::BlockId::IronOre;
		case voxel::Block::Obsidian: return ve::blocks::BlockId::Obsidian;
		case voxel::Block::OakLeaves: return ve::blocks::BlockId::OakLeaves;
		case voxel::Block::Deepslate: return ve::blocks::BlockId::Deepslate;
		case voxel::Block::CopperOre: return ve::blocks::BlockId::CopperOre;
		case voxel::Block::GoldOre: return ve::blocks::BlockId::GoldOre;
		case voxel::Block::EmeraldOre: return ve::blocks::BlockId::EmeraldOre;
		case voxel::Block::LapisOre: return ve::blocks::BlockId::LapisOre;
		case voxel::Block::Gravel: return ve::blocks::BlockId::Gravel;
		case voxel::Block::MossBlock: return ve::blocks::BlockId::MossBlock;
		case voxel::Block::Snow: return ve::blocks::BlockId::Snow;
		case voxel::Block::MossyCobblestone: return ve::blocks::BlockId::MossyCobblestone;
		case voxel::Block::Granite: return ve::blocks::BlockId::Granite;
		case voxel::Block::Andesite: return ve::blocks::BlockId::Andesite;
		case voxel::Block::Diorite: return ve::blocks::BlockId::Diorite;
		case voxel::Block::Blackstone: return ve::blocks::BlockId::Blackstone;
		case voxel::Block::Basalt: return ve::blocks::BlockId::Basalt;
		case voxel::Block::Bedrock: return ve::blocks::BlockId::Bedrock;
		case voxel::Block::BirchLog: return ve::blocks::BlockId::BirchLog;
		case voxel::Block::BirchPlanks: return ve::blocks::BlockId::BirchPlanks;
		case voxel::Block::BirchLeaves: return ve::blocks::BlockId::BirchLeaves;
		case voxel::Block::SpruceLog: return ve::blocks::BlockId::SpruceLog;
		case voxel::Block::SprucePlanks: return ve::blocks::BlockId::SprucePlanks;
		case voxel::Block::CraftingTable: return ve::blocks::BlockId::CraftingTable;
		case voxel::Block::Bookshelf: return ve::blocks::BlockId::Bookshelf;
		case voxel::Block::Pumpkin: return ve::blocks::BlockId::Pumpkin;
		case voxel::Block::Melon: return ve::blocks::BlockId::Melon;
		case voxel::Block::HayBlock: return ve::blocks::BlockId::HayBlock;
		case voxel::Block::AmethystBlock: return ve::blocks::BlockId::AmethystBlock;
		case voxel::Block::Glass: return ve::blocks::BlockId::Glass;
		case voxel::Block::Water: return ve::blocks::BlockId::Water;
		case voxel::Block::SeaLantern: return ve::blocks::BlockId::SeaLantern;
		case voxel::Block::CherryLog: return ve::blocks::BlockId::CherryLog;
		case voxel::Block::CherryPlanks: return ve::blocks::BlockId::CherryPlanks;
		case voxel::Block::CherryLeaves: return ve::blocks::BlockId::CherryLeaves;
		default: return ve::blocks::BlockId::Air;
		}
	}

	[[nodiscard]] ve::engine::WorldBlockEdit ToInternalWorldEdit(const voxel::WorldEdit& edit)
	{
		ve::engine::WorldBlockEdit result{};
		result.kind = edit.kind == voxel::WorldEdit::Kind::FillBox
			? ve::engine::WorldBlockEdit::Kind::FillBox
			: ve::engine::WorldBlockEdit::Kind::SetBlock;
		result.min_x = edit.kind == voxel::WorldEdit::Kind::FillBox ? edit.box.minimum.x : edit.position.x;
		result.min_y = edit.kind == voxel::WorldEdit::Kind::FillBox ? edit.box.minimum.y : edit.position.y;
		result.min_z = edit.kind == voxel::WorldEdit::Kind::FillBox ? edit.box.minimum.z : edit.position.z;
		result.max_x = edit.kind == voxel::WorldEdit::Kind::FillBox ? edit.box.maximum.x : edit.position.x;
		result.max_y = edit.kind == voxel::WorldEdit::Kind::FillBox ? edit.box.maximum.y : edit.position.y;
		result.max_z = edit.kind == voxel::WorldEdit::Kind::FillBox ? edit.box.maximum.z : edit.position.z;
		result.block = ToInternalBlock(edit.block);
		return result;
	}

	[[nodiscard]] glm::vec3 ToInternalVec3(voxel::Vec3 value) noexcept
	{
		return { value.x, value.y, value.z };
	}

	[[nodiscard]] ve::rendering::GraphicsApi ToInternalApi(voxel::GraphicsApi api) noexcept
	{
		switch (api)
		{
		case voxel::GraphicsApi::OpenGLCompatibility:
			return ve::rendering::GraphicsApi::OpenGLCompatibility;
		case voxel::GraphicsApi::DirectX12:
			return ve::rendering::GraphicsApi::DirectX12;
		case voxel::GraphicsApi::Vulkan:
		default:
			return ve::rendering::GraphicsApi::Vulkan;
		}
	}

	[[nodiscard]] ve::engine::EngineCreateInfo ToInternalConfig(const voxel::EngineConfig& config)
	{
		ve::engine::EngineCreateInfo result{};
		result.window.title = config.window.title;
		result.window.width = config.window.width;
		result.window.height = config.window.height;
		result.window.fullscreen = config.window.fullscreen;
		result.render_backend.preferred_api = ToInternalApi(config.graphics_api);
		result.camera_position = ToInternalVec3(config.camera.position);
		result.camera_look_at = ToInternalVec3(config.camera.look_at);
		result.has_custom_camera = config.camera.has_custom_view;
		result.vsync = config.window.vsync;
		result.show_debug_overlay = config.show_debug_overlay;
		result.world_size_chunks = config.world.size_chunks;
		result.render_distance_chunks = config.render_distance_chunks;
		result.world_edits.reserve(config.world.edits.size());
		for (const voxel::WorldEdit& edit : config.world.edits)
		{
			result.world_edits.push_back(ToInternalWorldEdit(edit));
		}
		return result;
	}
}

namespace voxel
{
	CameraConfig CameraConfig::At(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig{ position, look_at, true };
	}

	CameraConfig Camera(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig::At(position, look_at);
	}

	BlockBox BlockBox::Between(BlockPosition first, BlockPosition second) noexcept
	{
		return BlockBox{
			{
				std::min(first.x, second.x),
				std::min(first.y, second.y),
				std::min(first.z, second.z),
			},
			{
				std::max(first.x, second.x),
				std::max(first.y, second.y),
				std::max(first.z, second.z),
			}
		};
	}

	BlockBox Box(BlockPosition first, BlockPosition second) noexcept
	{
		return BlockBox::Between(first, second);
	}

	WorldConfig WorldConfig::SizeChunks(int value)
	{
		WorldConfig config{};
		config.size_chunks = value;
		return config;
	}

	WorldConfig& WorldConfig::WithSizeChunks(int value) noexcept
	{
		size_chunks = value;
		return *this;
	}

	WorldConfig World(int size_chunks)
	{
		return WorldConfig::SizeChunks(size_chunks);
	}

	WorldConfig World()
	{
		return {};
	}

	WorldConfig Scene(int size_chunks)
	{
		return World(size_chunks);
	}

	WorldConfig Scene()
	{
		return World();
	}

	WorldConfig& WorldConfig::SetBlock(BlockPosition position, Block block)
	{
		edits.push_back(WorldEdit{
			WorldEdit::Kind::SetBlock,
			position,
			{},
			block
		});
		return *this;
	}

	WorldConfig& WorldConfig::SetBlock(int x, int y, int z, Block block)
	{
		return SetBlock(At(x, y, z), block);
	}

	WorldConfig& WorldConfig::FillBox(BlockBox box, Block block)
	{
		edits.push_back(WorldEdit{
			WorldEdit::Kind::FillBox,
			{},
			box,
			block
		});
		return *this;
	}

	WorldConfig& WorldConfig::FillBox(BlockPosition first, BlockPosition second, Block block)
	{
		return FillBox(Box(first, second), block);
	}

	WorldConfig& WorldConfig::FillBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block)
	{
		return FillBox(At(x1, y1, z1), At(x2, y2, z2), block);
	}

	WorldConfig& WorldConfig::ClearBox(int x1, int y1, int z1, int x2, int y2, int z2)
	{
		return FillBox(x1, y1, z1, x2, y2, z2, Air);
	}

	WorldConfig& WorldConfig::AddFloor(int y, int radius, Block block)
	{
		return AddFloor(0, y, 0, radius, block);
	}

	WorldConfig& WorldConfig::AddFloor(int center_x, int y, int center_z, int radius, Block block)
	{
		return FillBox(
			center_x - radius,
			y,
			center_z - radius,
			center_x + radius,
			y,
			center_z + radius,
			block);
	}

	WorldConfig& WorldConfig::AddPlatform(int y, int radius, Block block)
	{
		return AddFloor(y, radius, block);
	}

	WorldConfig& WorldConfig::AddBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block)
	{
		return FillBox(x1, y1, z1, x2, y2, z2, block);
	}

	WorldConfig& WorldConfig::AddColumn(int x, int y, int z, int height, Block block)
	{
		if (height <= 0)
		{
			return *this;
		}
		return FillBox(x, y, z, x, y + height - 1, z, block);
	}

	WorldConfig& WorldConfig::AddLamp(int x, int y, int z, Block block)
	{
		return SetBlock(x, y, z, block);
	}

	WorldConfig& WorldConfig::AddLightPost(int x, int y, int z, int height, Block post, Block light)
	{
		if (height <= 0)
		{
			return AddLamp(x, y, z, light);
		}
		return AddColumn(x, y, z, height, post)
			.AddLamp(x, y + height, z, light);
	}

	WorldConfig& WorldConfig::AddPathX(int x1, int x2, int y, int z, int half_width, Block block)
	{
		return FillBox(x1, y, z - half_width, x2, y, z + half_width, block);
	}

	WorldConfig& WorldConfig::AddPathZ(int x, int y, int z1, int z2, int half_width, Block block)
	{
		return FillBox(x - half_width, y, z1, x + half_width, y, z2, block);
	}

	WorldConfig& WorldConfig::AddHouse(int x, int y, int z, int radius, Block walls, Block roof)
	{
		const int left = x - radius;
		const int right = x + radius;
		const int front = z - radius;
		const int back = z + radius;
		const int wall_top = y + 3;
		const int roof_y = y + 4;

		return AddBox(left, y, front, right, y, back, walls)
			.AddBox(left, y + 1, front, right, wall_top, front, walls)
			.AddBox(left, y + 1, back, right, wall_top, back, walls)
			.AddBox(left, y + 1, front, left, wall_top, back, walls)
			.AddBox(right, y + 1, front, right, wall_top, back, walls)
			.AddBox(left - 1, roof_y, front - 1, right + 1, roof_y, back + 1, roof)
			.ClearBox(x - 1, y + 1, front, x + 1, y + 2, front)
			.ClearBox(x - 1, y + 1, z - 1, x + 1, wall_top, z + 1)
			.AddLamp(x, wall_top, z);
	}

	WorldConfig& WorldConfig::AddTree(int x, int y, int z, Block trunk, Block leaves)
	{
		return AddColumn(x, y, z, 5, trunk)
			.AddBox(x - 2, y + 3, z - 2, x + 2, y + 5, z + 2, leaves)
			.AddBox(x - 1, y + 6, z - 1, x + 1, y + 6, z + 1, leaves);
	}

	WindowConfig WindowConfig::Windowed(std::string title, int width, int height)
	{
		WindowConfig config{};
		config.title = std::move(title);
		config.width = width;
		config.height = height;
		config.fullscreen = false;
		return config;
	}

	WindowConfig WindowConfig::Fullscreen(std::string title)
	{
		WindowConfig config{};
		config.title = std::move(title);
		config.fullscreen = true;
		return config;
	}

	WindowConfig& WindowConfig::EnableVSync(bool enabled) noexcept
	{
		vsync = enabled;
		return *this;
	}

	WindowConfig Window(std::string title, int width, int height)
	{
		return WindowConfig::Windowed(std::move(title), width, height);
	}

	WindowConfig Fullscreen(std::string title)
	{
		return WindowConfig::Fullscreen(std::move(title));
	}

	EngineConfig EngineConfig::Default()
	{
		return {};
	}

	EngineConfig EngineConfig::MinecraftDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine");
		config.world = WorldConfig::SizeChunks(8);
		config.graphics_api = GraphicsApi::Vulkan;
		config.render_distance_chunks = 2;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::WorldEditDemo()
	{
		return MinecraftDemo()
			.WithWindow(Window("Voxel API - World Edit", 1280, 720).EnableVSync())
			.WithWorld(World()
				.AddFloor(62, 8, Grass)
				.FillBox(-5, 63, -5, 5, 63, 5, OakPlanks)
				.FillBox(-2, 64, -2, 2, 67, 2, Glass)
				.SetBlock(0, 68, 0, SeaLantern))
			.WithCamera({ 18.0f, 72.0f, 18.0f }, { 0.0f, 64.0f, 0.0f })
			.WithRenderDistanceChunks(3);
	}

	EngineConfig EngineConfig::ShowcaseDemo()
	{
		return MinecraftDemo()
			.WithWindow(Window("Voxel API - Showcase", 1600, 900).EnableVSync())
			.WithWorld(World(10)
				.AddFloor(61, 10, Stone)
				.AddFloor(62, 10, MossBlock)
				.FillBox(-8, 63, -8, -3, 66, -3, Bricks)
				.FillBox(3, 63, -8, 8, 67, -3, CherryPlanks)
				.FillBox(-8, 63, 3, -3, 68, 8, AmethystBlock)
				.FillBox(3, 63, 3, 8, 69, 8, SeaLantern)
				.SetBlock(0, 63, 0, DiamondOre)
				.SetBlock(1, 63, 0, EmeraldOre)
				.SetBlock(-1, 63, 0, GoldOre))
			.WithCamera({ 24.0f, 76.0f, 28.0f }, { 0.0f, 64.0f, 0.0f })
			.WithRenderDistanceChunks(4)
			.ShowDebugOverlay(false);
	}

	EngineConfig MinecraftDemo()
	{
		return EngineConfig::MinecraftDemo();
	}

	EngineConfig WorldEditDemo()
	{
		return EngineConfig::WorldEditDemo();
	}

	EngineConfig ShowcaseDemo()
	{
		return EngineConfig::ShowcaseDemo();
	}

	EngineConfig DefaultConfig()
	{
		return EngineConfig::Default();
	}

	EngineConfig& EngineConfig::WithWindow(WindowConfig value) noexcept
	{
		window = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithWindow(std::string title, int width, int height)
	{
		return WithWindow(Window(std::move(title), width, height));
	}

	EngineConfig& EngineConfig::WithWorld(WorldConfig value) noexcept
	{
		world = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithScene(WorldConfig value) noexcept
	{
		return WithWorld(std::move(value));
	}

	EngineConfig& EngineConfig::WithCamera(CameraConfig value) noexcept
	{
		camera = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithCamera(Vec3 position, Vec3 look_at) noexcept
	{
		return WithCamera(Camera(position, look_at));
	}

	EngineConfig& EngineConfig::UseGraphicsApi(GraphicsApi value) noexcept
	{
		graphics_api = value;
		return *this;
	}

	EngineConfig& EngineConfig::UseVulkan() noexcept
	{
		return UseGraphicsApi(GraphicsApi::Vulkan);
	}

	EngineConfig& EngineConfig::UseOpenGLCompatibility() noexcept
	{
		return UseGraphicsApi(GraphicsApi::OpenGLCompatibility);
	}

	EngineConfig& EngineConfig::UseDirectX12() noexcept
	{
		return UseGraphicsApi(GraphicsApi::DirectX12);
	}

	EngineConfig& EngineConfig::WithWorldSizeChunks(int value) noexcept
	{
		world.size_chunks = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithRenderDistanceChunks(int value) noexcept
	{
		render_distance_chunks = value;
		return *this;
	}

	EngineConfig& EngineConfig::ShowDebugOverlay(bool enabled) noexcept
	{
		show_debug_overlay = enabled;
		return *this;
	}

	EngineConfig& EngineConfig::HideDebugOverlay() noexcept
	{
		return ShowDebugOverlay(false);
	}

	class Engine::Impl
	{
	public:
		explicit Impl(const EngineConfig& config)
			: engine_(ToInternalConfig(config))
		{
		}

		[[nodiscard]] int Run()
		{
			return engine_.Run();
		}

	private:
		::EngineApplication engine_;
	};

	Engine::Engine(EngineConfig config)
		: impl_(std::make_unique<Impl>(config))
	{
	}

	Engine::~Engine() = default;

	Engine::Engine(Engine&&) noexcept = default;

	Engine& Engine::operator=(Engine&&) noexcept = default;

	int Engine::Run()
	{
		return impl_->Run();
	}

	int Run(EngineConfig config)
	{
		Engine engine{ std::move(config) };
		return engine.Run();
	}

	int Run(WorldConfig world)
	{
		return Run(MinecraftDemo().WithWorld(std::move(world)));
	}

	int RunScene(WorldConfig scene)
	{
		return Run(std::move(scene));
	}

	int RunMinecraftDemo()
	{
		return Run(MinecraftDemo());
	}

	int RunWorldEditDemo()
	{
		return Run(WorldEditDemo());
	}

	int RunShowcaseDemo()
	{
		return Run(ShowcaseDemo());
	}
}
