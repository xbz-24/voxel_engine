#include "FluidGrid.h"

#include <algorithm>

namespace ve::simulation
{
	/** Creates a fluid grid with the requested dimensions. */
	FluidGrid::FluidGrid(int width, int height) { Resize(width, height); }

	/** Resizes the grid and resets all fields. */
	void FluidGrid::Resize(int width, int height)
	{
		width_ = std::max(width, 1);
		height_ = std::max(height, 1);
		density_.assign(static_cast<ve::core::Index>(width_ * height_), 0.0f);
		velocity_.assign(static_cast<ve::core::Index>(width_ * height_), glm::vec2{ 0.0f });
	}

	/** Clears density and velocity values. */
	void FluidGrid::Clear()
	{
		std::fill(density_.begin(), density_.end(), 0.0f);
		std::fill(velocity_.begin(), velocity_.end(), glm::vec2{ 0.0f });
	}

	/** Adds density to one cell. */
	void FluidGrid::AddDensity(int cell_x, int cell_y, float amount)
	{
		density_[CellIndex(cell_x, cell_y)] += amount;
	}

	/** Adds velocity to one cell. */
	void FluidGrid::AddVelocity(int cell_x, int cell_y, const glm::vec2& velocity)
	{
		velocity_[CellIndex(cell_x, cell_y)] += velocity;
	}

	/** Sets density for one cell. */
	void FluidGrid::SetDensity(int cell_x, int cell_y, float density)
	{
		density_[CellIndex(cell_x, cell_y)] = density;
	}

	/** Sets velocity for one cell. */
	void FluidGrid::SetVelocity(int cell_x, int cell_y, const glm::vec2& velocity)
	{
		velocity_[CellIndex(cell_x, cell_y)] = velocity;
	}

	/** Returns density from a clamped cell. */
	float FluidGrid::DensityAt(int cell_x, int cell_y) const noexcept
	{
		return density_[CellIndex(cell_x, cell_y)];
	}

	/** Returns velocity from a clamped cell. */
	glm::vec2 FluidGrid::VelocityAt(int cell_x, int cell_y) const noexcept
	{
		return velocity_[CellIndex(cell_x, cell_y)];
	}

	/** Returns the grid width. */
	int FluidGrid::Width() const noexcept { return width_; }

	/** Returns the grid height. */
	int FluidGrid::Height() const noexcept { return height_; }

	/** Converts cell coordinates to a clamped flat index. */
	ve::core::Index FluidGrid::CellIndex(int cell_x, int cell_y) const noexcept
	{
		const int clamped_x = std::clamp(cell_x, 0, width_ - 1);
		const int clamped_y = std::clamp(cell_y, 0, height_ - 1);
		return static_cast<ve::core::Index>(clamped_y * width_ + clamped_x);
	}
}
