		[[nodiscard]] std::optional<ve::blocks::SolidBlockColor> SampleMaterialColor(const ve::assets::ImportedModel& model,
			const ve::assets::ImportedMesh& mesh,
			glm::vec2 uv,
			TextureCache& texture_cache)
		{
			if (mesh.material_index >= model.materials.size()) return std::nullopt;
			const ve::assets::ImportedMaterial& material = model.materials[mesh.material_index];
			if (const ImportedTexture* texture = CachedTexture(texture_cache, material.albedo_texture))
			{
				const ve::blocks::SolidBlockColor sampled = SampleTextureColor(*texture, uv);
				if (HasUsefulTextureColor(sampled)) return sampled;
			}
			if (HasUsefulMaterialColor(material.base_color))
			{
				return ve::blocks::SolidBlockColor{ material.base_color.r, material.base_color.g, material.base_color.b, material.base_color.a };
			}
			return std::nullopt;
		}

		[[nodiscard]] BlockId BlockFromSourceColor(const ve::assets::ImportedModel& model, ve::core::Index material_index)
		{
			if (material_index >= model.materials.size()) return BlockId::Air;
			const glm::vec4 color = model.materials[material_index].base_color;
			return HasUsefulMaterialColor(color) ? ve::blocks::NearestSolidColorBlock({ color.r, color.g, color.b, color.a }) : BlockId::Air;
		}
