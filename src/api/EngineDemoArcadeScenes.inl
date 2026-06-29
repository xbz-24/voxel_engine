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
