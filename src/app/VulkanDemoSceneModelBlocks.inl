		[[nodiscard]] BlockId BlockForAquaTriangle(std::string_view hints, float height, std::uint32_t hash) noexcept
		{
			if (hints.find("hair") != std::string::npos || hints.find("blue") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::DiamondOre, BlockId::LapisOre, BlockId::Glass } }, hash);
			if (hints.find("skin") != std::string::npos || hints.find("face") != std::string::npos || hints.find("body") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::Sandstone, BlockId::CutSandstone, BlockId::BirchPlanks } }, hash);
			if (hints.find("cloth") != std::string::npos || hints.find("dress") != std::string::npos || hints.find("skirt") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::Snow, BlockId::Diorite, BlockId::DiamondOre } }, hash);
			if (height > 0.78f) return PickBlock(std::array<BlockId, 3>{ { BlockId::DiamondOre, BlockId::LapisOre, BlockId::Glass } }, hash);
			if (height > 0.58f) return PickBlock(std::array<BlockId, 3>{ { BlockId::Sandstone, BlockId::CutSandstone, BlockId::DiamondOre } }, hash);
			if (height > 0.34f) return PickBlock(std::array<BlockId, 4>{ { BlockId::Snow, BlockId::Diorite, BlockId::DiamondOre, BlockId::AmethystBlock } }, hash);
			return PickBlock(std::array<BlockId, 3>{ { BlockId::Blackstone, BlockId::DiamondOre, BlockId::Snow } }, hash);
		}

		[[nodiscard]] BlockId BlockForSponzaTriangle(std::string_view hints, float height, std::uint32_t hash) noexcept
		{
			if (hints.find("leaf") != std::string::npos || hints.find("plant") != std::string::npos || hints.find("green") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::MossBlock, BlockId::OakLeaves, BlockId::BirchLeaves } }, hash);
			if (hints.find("curtain") != std::string::npos || hints.find("cloth") != std::string::npos || hints.find("red") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::Terracotta, BlockId::RedSandstone, BlockId::Bricks } }, hash);
			if (hints.find("wood") != std::string::npos || hints.find("bark") != std::string::npos) return PickBlock(std::array<BlockId, 3>{ { BlockId::OakLog, BlockId::SpruceLog, BlockId::OakPlanks } }, hash);
			if (hints.find("roof") != std::string::npos || height > 0.82f) return PickBlock(std::array<BlockId, 3>{ { BlockId::Terracotta, BlockId::RedSandstone, BlockId::Blackstone } }, hash);
			return PickBlock(std::array<BlockId, 6>{ { BlockId::Stone, BlockId::Andesite, BlockId::MossyCobblestone, BlockId::Cobblestone, BlockId::Granite, BlockId::Blackstone } }, hash);
		}

		[[nodiscard]] BlockId BlockForImportedTriangle(const ve::assets::ImportedModel& model,
			const ve::assets::ImportedMesh& mesh,
			const glm::vec3& centroid,
			const glm::vec3& minimum,
			const glm::vec3& maximum,
			const std::optional<ve::blocks::SolidBlockColor>& sampled_color)
		{
			std::string hints = Lowercase(mesh.name);
			if (mesh.material_index < model.materials.size())
			{
				const ve::assets::ImportedMaterial& material = model.materials[mesh.material_index];
				hints += " " + Lowercase(material.name + " " + material.albedo_texture.string());
			}
			const std::string source = Lowercase(model.source_path.string());
			const float height = NormalizedHeight(centroid, minimum, maximum);
			const std::uint32_t hash = StableStringHash(hints) + static_cast<std::uint32_t>(height * 997.0f);
			const BlockId sampled_block = sampled_color ? ve::blocks::NearestSolidColorBlock(*sampled_color) : BlockId::Air;
			if (source.find("aqua") != std::string::npos || source.find("konosuba") != std::string::npos)
			{
				return sampled_block != BlockId::Air ? sampled_block : BlockForAquaTriangle(hints, height, hash);
			}
			if (source.find("sponza") != std::string::npos || source.find("atrium") != std::string::npos)
			{
				return sampled_block != BlockId::Air ? sampled_block : BlockForSponzaTriangle(hints, height, hash);
			}
			const BlockId source_color = sampled_block != BlockId::Air ? sampled_block : BlockFromSourceColor(model, mesh.material_index);
			if (source_color != BlockId::Air) return source_color;
			return PickBlock(std::array<BlockId, 4>{ { BlockId::Diorite, BlockId::Stone, BlockId::CutSandstone, BlockId::MossBlock } }, hash);
		}

		[[nodiscard]] bool ModelBounds(const ve::assets::ImportedModel& model, glm::vec3& minimum, glm::vec3& maximum)
		{
			minimum = glm::vec3{ std::numeric_limits<float>::max() };
			maximum = glm::vec3{ std::numeric_limits<float>::lowest() };
			bool has_vertex = false;
			for (const ve::assets::ImportedMesh& mesh : model.meshes)
			{
				for (const ve::assets::ImportedVertex& vertex : mesh.vertices)
				{
					minimum = glm::min(minimum, vertex.position);
					maximum = glm::max(maximum, vertex.position);
					has_vertex = true;
				}
			}
			return has_vertex;
		}

		[[nodiscard]] glm::vec3 ModelToVoxelPosition(const glm::vec3& position,
			const glm::vec3& minimum,
			const glm::vec3& target_origin,
			float scale) noexcept
		{
			return target_origin + ((position - minimum) * scale);
		}
