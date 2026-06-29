namespace
{
	class CountingModelImporter final : public ve::assets::IModelImporter
	{
	public:
		[[nodiscard]] std::string_view Name() const noexcept override
		{
			return "Counting";
		}

		[[nodiscard]] bool CanImport(const std::filesystem::path& model_path) const override
		{
			return model_path.extension() == ".counting";
		}

		[[nodiscard]] std::optional<ve::assets::ImportedModel> Import(const std::filesystem::path& model_path, const ve::assets::ModelImportOptions&) const override
		{
			++import_count;
			ve::assets::ImportedModel model{};
			model.source_path = model_path;
			return model;
		}

		static inline int import_count = 0;
	};
}
