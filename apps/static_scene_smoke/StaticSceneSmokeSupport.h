#pragma once

#include <voxel/Voxel.h>

#include <filesystem>

namespace static_scene_smoke
{
	class TriangleModelFixture
	{
	public:
		TriangleModelFixture();
		~TriangleModelFixture();

		TriangleModelFixture(const TriangleModelFixture&) = delete;
		TriangleModelFixture& operator=(const TriangleModelFixture&) = delete;

		[[nodiscard]] const std::filesystem::path& Path() const noexcept;

	private:
		std::filesystem::path path_;
	};

	[[nodiscard]] voxel::EngineConfig CreateConfig(
		const std::filesystem::path& assets,
		const std::filesystem::path& model_path,
		voxel::UpdateCallback on_update,
		voxel::LogCallback on_log = {});
}
