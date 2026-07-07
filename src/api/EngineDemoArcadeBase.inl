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
