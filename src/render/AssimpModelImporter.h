#pragma once

#include "ModelImporter.h"

struct aiMaterial;
struct aiMesh;

namespace ve::assets
{
	class AssimpModelImporter final : public IModelImporter
	{
	public:
		/** @return Human-readable importer name. */
		[[nodiscard]] std::string_view Name() const noexcept override;

		/** @param model_path Path to inspect. @return True when Assimp supports the extension. */
		[[nodiscard]] bool CanImport(const std::filesystem::path& model_path) const override;

		/** @param model_path Model path to load. @param options Import flags. @return Imported model or empty on failure. */
		[[nodiscard]] std::optional<ImportedModel> Import(const std::filesystem::path& model_path, const ModelImportOptions& options) const override;

	private:
		/** @param options Import options. @return Assimp post-process flags. */
		[[nodiscard]] static unsigned int BuildPostProcessFlags(const ModelImportOptions& options) noexcept;

		/** @param material Assimp material. @param model_directory Directory used to resolve relative textures. @return Internal material. */
		[[nodiscard]] static ImportedMaterial ReadMaterial(const aiMaterial& material, const std::filesystem::path& model_directory);

		/** @param mesh Assimp mesh. @return Internal mesh. */
		[[nodiscard]] static ImportedMesh ReadMesh(const aiMesh& mesh);
	};
}
