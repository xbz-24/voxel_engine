#pragma once

#include "ImportedModel.h"

#include <filesystem>
#include <optional>
#include <string_view>

namespace ve::assets
{
	class IModelImporter
	{
	public:
		// TODO: Add importer diagnostics with warnings, missing textures, scale corrections, and animation support.
		/** Releases importer implementation resources. */
		virtual ~IModelImporter() = default;

		/** @return Human-readable importer name. */
		[[nodiscard]] virtual std::string_view Name() const noexcept = 0;

		/** @param model_path Path to inspect. @return True when this importer supports it. */
		[[nodiscard]] virtual bool CanImport(const std::filesystem::path& model_path) const = 0;

		/** @param model_path Model path to load. @param options Import flags. @return Imported model or empty on failure. */
		[[nodiscard]] virtual std::optional<ImportedModel> Import(const std::filesystem::path& model_path, const ModelImportOptions& options) const = 0;
	};
}
