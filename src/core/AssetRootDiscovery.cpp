#include "AssetRootDiscovery.h"

#include "ExecutablePath.h"

#include <system_error>

namespace ve::assets::detail
{
	namespace
	{
		[[nodiscard]] bool HasAssetDirectory(const std::filesystem::path& root_directory)
		{
			std::error_code error;
			return std::filesystem::is_directory(root_directory / "assets", error);
		}

		[[nodiscard]] std::filesystem::path NormalizePath(const std::filesystem::path& path)
		{
			std::error_code error;
			const std::filesystem::path absolute_path = std::filesystem::absolute(path, error);
			const std::filesystem::path normalized_path = error ? path : absolute_path;
			return normalized_path.lexically_normal();
		}

		[[nodiscard]] bool IsAssetDirectory(const std::filesystem::path& path)
		{
			if (path.filename() == "assets") return true;
			std::error_code error;
			return std::filesystem::is_directory(path / "textures", error) ||
				std::filesystem::is_directory(path / "models", error);
		}

		[[nodiscard]] std::filesystem::path NormalizeSearchRoot(const std::filesystem::path& search_root)
		{
			const std::filesystem::path normalized_root = NormalizePath(search_root);
			return IsAssetDirectory(normalized_root) ? normalized_root.parent_path() : normalized_root;
		}

		[[nodiscard]] std::filesystem::path FindAncestorWithAssets(std::filesystem::path start_directory)
		{
			if (start_directory.empty()) return {};
			for (std::filesystem::path current_directory = NormalizePath(start_directory); !current_directory.empty();)
			{
				if (HasAssetDirectory(current_directory)) return current_directory;
				const std::filesystem::path parent_directory = current_directory.parent_path();
				if (parent_directory == current_directory) break;
				current_directory = parent_directory;
			}
			return {};
		}

		[[nodiscard]] std::filesystem::path DevelopmentRootDirectory()
		{
#if defined(ROOT_DIR)
			return std::filesystem::path{ ROOT_DIR };
#else
			return {};
#endif
		}
	}

	std::filesystem::path ResolveRootDirectory(const AssetPathResolveOptions& options)
	{
		for (const std::filesystem::path& search_root : options.search_roots)
		{
			if (!search_root.empty()) return NormalizeSearchRoot(search_root);
		}

		const std::filesystem::path executable_asset_root = FindAncestorWithAssets(ve::core::ExecutableDirectory());
		if (!executable_asset_root.empty()) return executable_asset_root;

		const std::filesystem::path development_root = DevelopmentRootDirectory();
		if (!development_root.empty()) return NormalizePath(development_root);

		return NormalizePath(std::filesystem::current_path());
	}
}
