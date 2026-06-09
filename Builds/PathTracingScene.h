#pragma once

#include "CoreTypes.h"
#include "PathTracingTypes.h"

#include <span>

namespace ve::rendering
{
	/** Owns simple analytic geometry consumed by the path tracer. */
	class PathTracingScene
	{
	public:
		/** @param sphere Sphere copied into the scene. */
		void AddSphere(const PathTracingSphere& sphere);

		/** Removes all analytic geometry. */
		void Clear();

		/** @return Read-only scene sphere list. */
		[[nodiscard]] std::span<const PathTracingSphere> Spheres() const noexcept;

	private:
		ve::core::DynamicArray<PathTracingSphere> spheres_;
	};
}
