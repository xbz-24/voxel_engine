#pragma once

#include "CoreTypes.h"

#include <glm/glm.hpp>

namespace ve::simulation
{
	/** Stores density and velocity fields for a 2D fluid grid. */
	class FluidGrid
	{
	public:
		/** @param width Cell count on X. @param height Cell count on Y. */
		explicit FluidGrid(int width = 1, int height = 1);

		/** @param width New X cell count. @param height New Y cell count. */
		void Resize(int width, int height);

		/** Clears density and velocity values to zero. */
		void Clear();

		/** @param cell_x Cell X. @param cell_y Cell Y. @param amount Density amount added to the cell. */
		void AddDensity(int cell_x, int cell_y, float amount);

		/** @param cell_x Cell X. @param cell_y Cell Y. @param velocity Velocity amount added to the cell. */
		void AddVelocity(int cell_x, int cell_y, const glm::vec2& velocity);

		/** @param cell_x Cell X. @param cell_y Cell Y. @param density New density value. */
		void SetDensity(int cell_x, int cell_y, float density);

		/** @param cell_x Cell X. @param cell_y Cell Y. @param velocity New velocity value. */
		void SetVelocity(int cell_x, int cell_y, const glm::vec2& velocity);

		/** @param cell_x Cell X. @param cell_y Cell Y. @return Density sampled at a clamped cell. */
		[[nodiscard]] float DensityAt(int cell_x, int cell_y) const noexcept;

		/** @param cell_x Cell X. @param cell_y Cell Y. @return Velocity sampled at a clamped cell. */
		[[nodiscard]] glm::vec2 VelocityAt(int cell_x, int cell_y) const noexcept;

		/** @param position Continuous grid position. @return Bilinear density sample. */
		[[nodiscard]] float SampleDensity(const glm::vec2& position) const noexcept;

		/** @param position Continuous grid position. @return Bilinear velocity sample. */
		[[nodiscard]] glm::vec2 SampleVelocity(const glm::vec2& position) const noexcept;

		/** @return Width in cells. */
		[[nodiscard]] int Width() const noexcept;

		/** @return Height in cells. */
		[[nodiscard]] int Height() const noexcept;

	private:
		/** @param cell_x Cell X. @param cell_y Cell Y. @return Clamped flat cell index. */
		[[nodiscard]] ve::core::Index CellIndex(int cell_x, int cell_y) const noexcept;

		int width_ = 1;
		int height_ = 1;
		ve::core::DynamicArray<float> density_;
		ve::core::DynamicArray<glm::vec2> velocity_;
	};
}
