#include "voxel/Engine.h"

#include <cstddef>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>

namespace voxel
{
	namespace
	{
		class IWorldSceneBuilder
		{
		public:
			virtual ~IWorldSceneBuilder() = default;

			[[nodiscard]] virtual WorldConfig Build() const = 0;
		};

		class WorldEditDemoSceneBuilder final : public IWorldSceneBuilder
		{
		public:
			[[nodiscard]] WorldConfig Build() const override
			{
				return Scene()
					.AddPlatform(62, 10, Grass)
					.AddHouse(0, 63, 0, 4, OakPlanks, Bricks)
					.AddGarden(-9, 63, 4)
					.AddPond(9, 63, 4)
					.AddBridgeX(5, 12, 64, 4)
					.AddLightPost(-5, 63, -7)
					.AddLightPost(5, 63, -7)
					.AddTree(-10, 63, -7)
					.AddTree(10, 63, -7, BirchLog, BirchLeaves);
			}
		};

		class ShowcaseDemoSceneBuilder final : public IWorldSceneBuilder
		{
		public:
			[[nodiscard]] WorldConfig Build() const override
			{
				return Scene(10)
					.AddPlatform(61, 14, Stone)
					.AddPlatform(62, 13, MossBlock)
					.AddPathZ(0, 63, -14, 14)
					.AddPathX(-14, 14, 63, 0)
					.AddHouse(-8, 63, -8, 3, Bricks, Blackstone)
					.AddHouse(8, 63, -8, 3, CherryPlanks, CherryLeaves)
					.AddPond(-8, 63, 8, 4)
					.AddGarden(8, 63, 7, 3, 4)
					.AddLightPost(-4, 63, -4)
					.AddLightPost(4, 63, -4)
					.AddLightPost(-4, 63, 4)
					.AddLightPost(4, 63, 4)
					.SetBlock(0, 64, 0, DiamondOre)
					.SetBlock(1, 64, 0, EmeraldOre)
					.SetBlock(-1, 64, 0, GoldOre);
			}
		};

		struct ArcadeBoard
		{
			int origin_x = -28;
			int origin_z = -18;
			int base_y = 64;
			int width = 28;
			int height = 18;
			int pixel = 2;
		};

		class ArcadeCanvas
		{
		public:
			ArcadeCanvas(WorldConfig& world, ArcadeBoard board) noexcept
				: world_(world),
				  board_(board)
			{
			}

			ArcadeCanvas& Arena()
			{
				const int right = board_.origin_x + (board_.width * board_.pixel) - 1;
				const int back = board_.origin_z + (board_.height * board_.pixel) - 1;
				world_
					.FillBox(board_.origin_x - 2, board_.base_y - 1, board_.origin_z - 2, right + 2, board_.base_y - 1, back + 2, Blackstone)
					.FillBox(board_.origin_x - 1, board_.base_y, board_.origin_z - 1, right + 1, board_.base_y, back + 1, Deepslate)
					.FillBox(board_.origin_x, board_.base_y + 1, board_.origin_z, right, board_.base_y + 1, back, Obsidian)
					.FillBox(board_.origin_x - 2, board_.base_y + 1, board_.origin_z - 2, right + 2, board_.base_y + 3, board_.origin_z - 1, SeaLantern)
					.FillBox(board_.origin_x - 2, board_.base_y + 1, back + 1, right + 2, board_.base_y + 3, back + 2, SeaLantern)
					.FillBox(board_.origin_x - 2, board_.base_y + 1, board_.origin_z - 2, board_.origin_x - 1, board_.base_y + 3, back + 2, SeaLantern)
					.FillBox(right + 1, board_.base_y + 1, board_.origin_z - 2, right + 2, board_.base_y + 3, back + 2, SeaLantern);
				return *this;
			}

			ArcadeCanvas& Pixel(int x, int y, Block block, int layer = 0)
			{
				const int world_x = board_.origin_x + (x * board_.pixel);
				const int world_z = board_.origin_z + (y * board_.pixel);
				const int world_y = board_.base_y + 2 + layer;
				world_.FillBox(world_x, world_y, world_z, world_x + board_.pixel - 1, world_y, world_z + board_.pixel - 1, block);
				return *this;
			}

			ArcadeCanvas& Rect(int x, int y, int width, int height, Block block, int layer = 0)
			{
				for (int row = 0; row < height; ++row)
				{
					for (int column = 0; column < width; ++column)
					{
						Pixel(x + column, y + row, block, layer);
					}
				}
				return *this;
			}

			ArcadeCanvas& Sprite(int x, int y, std::initializer_list<std::string_view> rows, Block block, int layer = 0)
			{
				int row_index = 0;
				for (const std::string_view row : rows)
				{
					for (int column = 0; column < static_cast<int>(row.size()); ++column)
					{
						if (row[static_cast<std::size_t>(column)] != ' ')
						{
							Pixel(x + column, y + row_index, block, layer);
						}
					}
					++row_index;
				}
				return *this;
			}

			[[nodiscard]] int Width() const noexcept { return board_.width; }
			[[nodiscard]] int Height() const noexcept { return board_.height; }

		private:
			WorldConfig& world_;
			ArcadeBoard board_;
		};

		class ArcadeSceneBuilder : public IWorldSceneBuilder
		{
		public:
			[[nodiscard]] WorldConfig Build() const final
			{
				WorldConfig world = Scene(8);
				ArcadeCanvas canvas{ world, Board() };
				canvas.Arena();
				Draw(canvas);
				AddLights(world);
				return world;
			}

		protected:
			[[nodiscard]] virtual ArcadeBoard Board() const noexcept
			{
				return {};
			}

			virtual void Draw(ArcadeCanvas& canvas) const = 0;

			virtual void AddLights(WorldConfig& world) const
			{
				world.AddLightPost(-34, 64, -24).AddLightPost(34, 64, 24);
			}
		};

		class ArcadeSnakeSceneBuilder final : public ArcadeSceneBuilder
		{
		private:
			void Draw(ArcadeCanvas& canvas) const override
			{
				canvas.Rect(4, 4, 5, 2, MossBlock, 1)
					.Rect(8, 6, 2, 4, MossBlock, 1)
					.Rect(10, 9, 5, 2, MossBlock, 1)
					.Rect(15, 7, 2, 3, MossBlock, 1)
					.Rect(17, 6, 3, 2, MossBlock, 1)
					.Pixel(21, 5, DiamondOre, 2)
					.Pixel(6, 13, EmeraldOre, 2);
			}
		};

		class ArcadePaddleSceneBuilder final : public ArcadeSceneBuilder
		{
		private:
			void Draw(ArcadeCanvas& canvas) const override
			{
				for (int y = 1; y < canvas.Height() - 1; y += 2)
				{
					canvas.Pixel(canvas.Width() / 2, y, Glass, 1);
				}
				canvas.Rect(3, 6, 1, 6, OakPlanks, 2)
					.Rect(24, 4, 1, 6, CherryPlanks, 2)
					.Rect(13, 8, 2, 2, SeaLantern, 3)
					.Rect(7, 2, 2, 3, DiamondOre, 2)
					.Rect(19, 13, 2, 3, EmeraldOre, 2);
			}

			void AddLights(WorldConfig& world) const override
			{
				world.AddLightPost(-34, 64, 0).AddLightPost(34, 64, 0);
			}
		};

		class ArcadeBlocksSceneBuilder final : public ArcadeSceneBuilder
		{
		private:
			[[nodiscard]] ArcadeBoard Board() const noexcept override
			{
				return { -20, -18, 64, 20, 18, 2 };
			}

			void Draw(ArcadeCanvas& canvas) const override
			{
				canvas.Rect(8, 1, 4, 1, Glass, 3)
					.Rect(10, 2, 1, 3, Glass, 3)
					.Rect(3, 13, 4, 2, MossBlock, 2)
					.Rect(7, 12, 2, 3, CherryPlanks, 2)
					.Rect(9, 14, 5, 2, GoldOre, 2)
					.Rect(14, 11, 2, 5, DiamondOre, 2)
					.Rect(16, 15, 3, 1, EmeraldOre, 2);
			}

			void AddLights(WorldConfig& world) const override
			{
				world.AddLightPost(-26, 64, -24).AddLightPost(26, 64, 24);
			}
		};

		class ArcadeInvadersSceneBuilder final : public ArcadeSceneBuilder
		{
		private:
			void Draw(ArcadeCanvas& canvas) const override
			{
				for (int row = 0; row < 3; ++row)
				{
					for (int column = 0; column < 5; ++column)
					{
						canvas.Sprite(4 + (column * 4), 3 + (row * 3), { " X ", "XXX", "X X" }, row == 0 ? EmeraldOre : MossBlock, 2);
					}
				}
				canvas.Rect(13, 15, 3, 1, SeaLantern, 3)
					.Pixel(14, 14, SeaLantern, 3)
					.Pixel(6, 12, Glass, 2)
					.Pixel(18, 10, Glass, 2);
			}
		};

		class ArcadeMazeSceneBuilder final : public ArcadeSceneBuilder
		{
		private:
			void Draw(ArcadeCanvas& canvas) const override
			{
				for (int x = 2; x < canvas.Width() - 2; x += 4)
				{
					canvas.Rect(x, 2, 1, 12, Blackstone, 2);
				}
				canvas.Rect(3, 4, 8, 1, Blackstone, 2)
					.Rect(14, 7, 8, 1, Blackstone, 2)
					.Rect(5, 13, 18, 1, Blackstone, 2)
					.Pixel(4, 3, GoldOre, 3)
					.Pixel(8, 11, GoldOre, 3)
					.Pixel(19, 5, GoldOre, 3)
					.Sprite(12, 9, { " X ", "XXX", " X " }, SeaLantern, 4)
					.Sprite(22, 3, { "XXX", "X X", "XXX" }, CherryLeaves, 3);
			}
		};

		[[nodiscard]] EngineConfig ArcadeConfig(std::string title, WorldConfig world)
		{
			return EngineConfig::MinecraftDemo()
				.WithWindow(Window(std::move(title), 960, 720).EnableVSync())
				.WithScene(std::move(world))
				.WithCamera({ 0.0f, 105.0f, 58.0f }, { 0.0f, 66.0f, 0.0f })
				.WithRenderDistanceChunks(3)
				.ShowDebugOverlay(false);
		}
	}

	EngineConfig EngineConfig::DesertDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Crystal Jungle");
		config.world = WorldConfig::SizeChunks(20);
		config.camera = CameraConfig::At({ 154.0f, 82.0f, 292.0f }, { 160.0f, 52.0f, 86.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::HyperrealDesert;
		config.render_distance_chunks = 16;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::AquaModelDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Aqua Model Preview");
		config.world = WorldConfig::SizeChunks(16);
		config.camera = CameraConfig::At({ 120.0f, 82.0f, 168.0f }, { 92.0f, 58.0f, 92.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::AquaModel;
		config.render_distance_chunks = 12;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::SponzaAtriumDemo()
	{
		EngineConfig config{};
		config.window = WindowConfig::Windowed("Voxel Engine - Sponza Atrium Preview");
		config.world = WorldConfig::SizeChunks(20);
		config.camera = CameraConfig::At({ 176.0f, 90.0f, 210.0f }, { 128.0f, 58.0f, 116.0f });
		config.graphics_api = GraphicsApi::Vulkan;
		config.demo_scene = DemoScene::SponzaAtrium;
		config.render_distance_chunks = 16;
		config.show_debug_overlay = true;
		return config;
	}

	EngineConfig EngineConfig::MinecraftDemo()
	{
		return DesertDemo();
	}

	EngineConfig EngineConfig::WorldEditDemo()
	{
		return MinecraftDemo()
			.WithWindow(Window("Voxel API - World Edit", 1280, 720).EnableVSync())
			.WithScene(WorldEditDemoSceneBuilder{}.Build())
			.WithCamera({ 18.0f, 72.0f, 18.0f }, { 0.0f, 64.0f, 0.0f })
			.WithRenderDistanceChunks(3);
	}

	EngineConfig EngineConfig::ShowcaseDemo()
	{
		return MinecraftDemo()
			.WithWindow(Window("Voxel API - Showcase", 1600, 900).EnableVSync())
			.WithScene(ShowcaseDemoSceneBuilder{}.Build())
			.WithCamera({ 24.0f, 76.0f, 28.0f }, { 0.0f, 64.0f, 0.0f })
			.WithRenderDistanceChunks(4)
			.ShowDebugOverlay(false);
	}

	EngineConfig EngineConfig::ArcadeSnakeDemo()
	{
		return ArcadeConfig("Voxel Arcade - Snake Trail", ArcadeSnakeSceneBuilder{}.Build());
	}

	EngineConfig EngineConfig::ArcadePaddleDemo()
	{
		return ArcadeConfig("Voxel Arcade - Paddle Duel", ArcadePaddleSceneBuilder{}.Build());
	}

	EngineConfig EngineConfig::ArcadeBlocksDemo()
	{
		return ArcadeConfig("Voxel Arcade - Falling Blocks", ArcadeBlocksSceneBuilder{}.Build());
	}

	EngineConfig EngineConfig::ArcadeInvadersDemo()
	{
		return ArcadeConfig("Voxel Arcade - Star Defenders", ArcadeInvadersSceneBuilder{}.Build());
	}

	EngineConfig EngineConfig::ArcadeMazeDemo()
	{
		return ArcadeConfig("Voxel Arcade - Neon Maze", ArcadeMazeSceneBuilder{}.Build());
	}

	EngineConfig DesertDemo()
	{
		return EngineConfig::DesertDemo();
	}

	EngineConfig AquaModelDemo()
	{
		return EngineConfig::AquaModelDemo();
	}

	EngineConfig SponzaAtriumDemo()
	{
		return EngineConfig::SponzaAtriumDemo();
	}

	EngineConfig MinecraftDemo()
	{
		return DesertDemo();
	}

	EngineConfig WorldEditDemo()
	{
		return EngineConfig::WorldEditDemo();
	}

	EngineConfig ShowcaseDemo()
	{
		return EngineConfig::ShowcaseDemo();
	}

	EngineConfig ArcadeSnakeDemo()
	{
		return EngineConfig::ArcadeSnakeDemo();
	}

	EngineConfig ArcadePaddleDemo()
	{
		return EngineConfig::ArcadePaddleDemo();
	}

	EngineConfig ArcadeBlocksDemo()
	{
		return EngineConfig::ArcadeBlocksDemo();
	}

	EngineConfig ArcadeInvadersDemo()
	{
		return EngineConfig::ArcadeInvadersDemo();
	}

	EngineConfig ArcadeMazeDemo()
	{
		return EngineConfig::ArcadeMazeDemo();
	}

	EngineConfig DefaultConfig()
	{
		return EngineConfig::Default();
	}
}
