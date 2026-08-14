#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

namespace ve::tests
{
	class TemporaryStaticObj
	{
	public:
		TemporaryStaticObj()
		{
			const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
			root_ = std::filesystem::temp_directory_path() /
				("voxel_static_scene_" + std::to_string(nonce));
			std::filesystem::create_directories(root_);
			path_ = root_ / "triangle.obj";
			std::ofstream output{ path_ };
			output << "o Triangle\n"
				"v 0 0 0\n"
				"v 1 0 0\n"
				"v 0 1 0\n"
				"f 1 2 3\n";
		}

		~TemporaryStaticObj()
		{
			std::error_code error;
			std::filesystem::remove_all(root_, error);
		}

		TemporaryStaticObj(const TemporaryStaticObj&) = delete;
		TemporaryStaticObj& operator=(const TemporaryStaticObj&) = delete;
		[[nodiscard]] const std::filesystem::path& Path() const noexcept { return path_; }

	private:
		std::filesystem::path root_;
		std::filesystem::path path_;
	};
}
