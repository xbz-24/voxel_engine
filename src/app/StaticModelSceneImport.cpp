#include "StaticModelSceneImport.h"

#include "ImportedModel.h"
#include "ModelAssetLibrary.h"

#include <algorithm>
#include <iterator>
#include <optional>
#include <utility>

namespace ve::engine
{
	namespace
	{
		[[nodiscard]] EngineStartupResult InvalidMesh(std::string message)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::StaticModelValidationFailed, std::move(message));
		}

		[[nodiscard]] ve::rendering::Vertex3D ConvertVertex(
			const ve::assets::ImportedVertex& vertex) noexcept
		{
			return ve::rendering::Vertex3D{
				vertex.position,
				vertex.normal,
				vertex.texture_coordinates,
				ve::rendering::ColorRgba{
					vertex.color.r, vertex.color.g, vertex.color.b, vertex.color.a }
			};
		}

		[[nodiscard]] bool HasIdentityTransforms(const ve::assets::ImportedNode& node) noexcept
		{
			for (int column = 0; column < 4; ++column)
			{
				for (int row = 0; row < 4; ++row)
				{
					const float expected = column == row ? 1.0f : 0.0f;
					if (node.transform[column][row] != expected) return false;
				}
			}
			return std::ranges::all_of(node.children, HasIdentityTransforms);
		}
	}

	EngineStartupResult ConvertStaticModelSceneMesh(
		const ve::assets::ImportedModel& model,
		StaticModelSceneMeshData& mesh_data)
	{
		mesh_data = {};
		if (model.meshes.size() != 1)
			return InvalidMesh("Static model scene must contain exactly one mesh");
		if (model.source_animation_count != 0)
			return InvalidMesh("Static model scene must not contain animations");
		if (model.source_camera_count != 0)
			return InvalidMesh("Static model scene must not contain cameras");
		if (model.source_light_count != 0)
			return InvalidMesh("Static model scene must not contain lights");
		if (model.has_root_node && !HasIdentityTransforms(model.root_node))
			return InvalidMesh("Static model scene nodes must use identity transforms");
		const ve::assets::ImportedMesh& mesh = model.meshes.front();
		if (!mesh.joints.empty())
			return InvalidMesh("Static model scene mesh must not contain skinning joints");
		if (mesh.vertices.empty())
			return InvalidMesh("Static model scene mesh must contain vertices");
		if (mesh.indices.empty() || mesh.indices.size() % 3 != 0)
			return InvalidMesh("Static model scene mesh indices must describe complete triangles");
		if (std::ranges::any_of(mesh.indices,
			[&mesh](std::uint32_t index) { return index >= mesh.vertices.size(); }))
		{
			return InvalidMesh("Static model scene mesh contains an out-of-range index");
		}

		mesh_data.vertices.clear();
		mesh_data.vertices.reserve(mesh.vertices.size());
		std::ranges::transform(mesh.vertices,
			std::back_inserter(mesh_data.vertices), ConvertVertex);
		mesh_data.indices = mesh.indices;
		return EngineStartupResult::Success();
	}

	EngineStartupResult ImportStaticModelScene(
		const std::filesystem::path& model_path,
		StaticModelSceneMeshData& mesh_data)
	{
		ve::assets::ModelAssetLibrary library;
		const std::optional<ve::assets::ImportedModel> model = library.ImportModel(model_path);
		if (!model)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::StaticModelImportFailed,
				"Static model OBJ could not be imported: " + model_path.string());
		}
		return ConvertStaticModelSceneMesh(*model, mesh_data);
	}
}
