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

		/** @param x Cell X. @param y Cell Y. @param amount Density amount added to the cell. */
		void AddDensity(int x, int y, float amount);

		/** @param x Cell X. @param y Cell Y. @param velocity Velocity amount added to the cell. */
		void AddVelocity(int x, int y, const glm::vec2& velocity);

		/** @param x Cell X. @param y Cell Y. @param density New density value. */
		void SetDensity(int x, int y, float density);

		/** @param x Cell X. @param y Cell Y. @param velocity New velocity value. */
		void SetVelocity(int x, int y, const glm::vec2& velocity);

		/** @param x Cell X. @param y Cell Y. @return Density sampled at a clamped cell. */
		[[nodiscard]] float DensityAt(int x, int y) const noexcept;

		/** @param x Cell X. @param y Cell Y. @return Velocity sampled at a clamped cell. */
		[[nodiscard]] glm::vec2 VelocityAt(int x, int y) const noexcept;

		/** @param position Continuous grid position. @return Bilinear density sample. */
		[[nodiscard]] float SampleDensity(const glm::vec2& position) const noexcept;

		/** @param position Continuous grid position. @return Bilinear velocity sample. */
		[[nodiscard]] glm::vec2 SampleVelocity(const glm::vec2& position) const noexcept;

		/** @return Width in cells. */
		[[nodiscard]] int Width() const noexcept;

		/** @return Height in cells. */
		[[nodiscard]] int Height() const noexcept;

	private:
		/** @param x Cell X. @param y Cell Y. @return Clamped flat cell index. */
		[[nodiscard]] ve::core::Index CellIndex(int x, int y) const noexcept;

		int width_ = 1;
		int height_ = 1;
		ve::core::DynamicArray<float> density_;
		ve::core::DynamicArray<glm::vec2> velocity_;
	};
}
