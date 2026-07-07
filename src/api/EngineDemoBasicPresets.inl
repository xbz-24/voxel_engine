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

