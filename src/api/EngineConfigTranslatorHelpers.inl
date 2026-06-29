		[[nodiscard]] ve::engine::WorldBlockEdit ToInternalWorldEdit(const WorldEdit& edit)
		{
			ve::engine::WorldBlockEdit result{};
			result.kind = edit.kind == WorldEdit::Kind::FillBox
				? ve::engine::WorldBlockEdit::Kind::FillBox
				: ve::engine::WorldBlockEdit::Kind::SetBlock;
			result.min_x = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.x : edit.position.x;
			result.min_y = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.y : edit.position.y;
			result.min_z = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.z : edit.position.z;
			result.max_x = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.x : edit.position.x;
			result.max_y = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.y : edit.position.y;
			result.max_z = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.z : edit.position.z;
			result.block = ToInternalBlock(edit.block);
			return result;
		}

		[[nodiscard]] glm::vec3 ToInternalVec3(Vec3 value) noexcept
		{
			return { value.x, value.y, value.z };
		}

		[[nodiscard]] ve::rendering::GraphicsApi ToInternalApi(GraphicsApi api) noexcept
		{
			switch (api)
			{
			case GraphicsApi::OpenGLCompatibility:
				return ve::rendering::GraphicsApi::OpenGLCompatibility;
			case GraphicsApi::DirectX12:
				return ve::rendering::GraphicsApi::DirectX12;
			case GraphicsApi::Vulkan:
			default:
				return ve::rendering::GraphicsApi::Vulkan;
			}
		}

		[[nodiscard]] ve::rendering::VulkanMinecraftDemoPreset ToInternalDemoPreset(DemoScene scene) noexcept
		{
			switch (scene)
			{
			case DemoScene::AquaModel:
				return ve::rendering::VulkanMinecraftDemoPreset::AquaModel;
			case DemoScene::SponzaAtrium:
				return ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium;
			case DemoScene::HyperrealDesert:
			default:
				return ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert;
			}
		}
