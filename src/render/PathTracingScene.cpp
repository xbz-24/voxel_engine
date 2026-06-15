#include "PathTracingScene.h"

namespace ve::rendering
{
	/** Adds a sphere to the scene. */
	void PathTracingScene::AddSphere(const PathTracingSphere& sphere)
	{
		spheres_.push_back(sphere);
	}

	/** Clears scene geometry. */
	void PathTracingScene::Clear()
	{
		spheres_.clear();
	}

	/** Returns read-only sphere geometry. */
	std::span<const PathTracingSphere> PathTracingScene::Spheres() const noexcept
	{
		return spheres_;
	}
}
