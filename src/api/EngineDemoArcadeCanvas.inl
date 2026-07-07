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
