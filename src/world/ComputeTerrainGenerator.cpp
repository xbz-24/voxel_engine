#include "ComputeTerrainGenerator.h"

namespace ve::world::generation
{
	/// Creates a compute terrain dispatcher.
	ComputeTerrainGenerator::ComputeTerrainGenerator(ve::rendering::ComputeProgramHandle compute_program,
		const ve::rendering::ComputeDispatcher* dispatcher) noexcept
		: compute_program_(compute_program),
		  dispatcher_(dispatcher)
	{
	}

	/// Sets the OpenGL compute shader program used for terrain generation.
	void ComputeTerrainGenerator::SetProgram(ve::rendering::ComputeProgramHandle compute_program) noexcept
	{
		compute_program_ = compute_program;
	}

	void ComputeTerrainGenerator::SetDispatcher(const ve::rendering::ComputeDispatcher* dispatcher) noexcept
	{
		dispatcher_ = dispatcher;
	}

	/// Reports whether compute shaders are supported by the active backend.
	bool ComputeTerrainGenerator::IsSupported() const noexcept
	{
		return dispatcher_ != nullptr && dispatcher_->IsSupported();
	}

	/// Reports whether a compute program has been assigned.
	bool ComputeTerrainGenerator::HasProgram() const noexcept
	{
		return compute_program_.IsValid();
	}

	/// Dispatches terrain generation work groups and flushes shader writes.
	void ComputeTerrainGenerator::Dispatch(std::uint32_t groups_x, std::uint32_t groups_y, std::uint32_t groups_z) const
	{
		// TODO: Return dispatch success/failure so callers can fall back to CPU terrain generation deterministically.
		if (!IsSupported() || !HasProgram()) return;
		dispatcher_->Dispatch(compute_program_, { groups_x, groups_y, groups_z });
	}
}
