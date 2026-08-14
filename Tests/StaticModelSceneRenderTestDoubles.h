#pragma once

#include "IndexedTriangleMesh.h"
#include "RenderBackend.h"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace ve::tests
{
	struct StaticMeshResourceState
	{
		std::vector<ve::rendering::Vertex3D> vertices;
		std::vector<std::uint32_t> indices;
		glm::mat4 model_matrix{ 0.0f };
		int upload_count = 0;
		int draw_count = 0;
		int release_count = 0;
		int creation_attempts = 0;
	};

	class RecordingIndexedTriangleMesh final : public ve::rendering::IndexedTriangleMesh
	{
	public:
		explicit RecordingIndexedTriangleMesh(
			std::shared_ptr<StaticMeshResourceState> state) noexcept
			: state_(std::move(state)) {}

		bool Upload(const ve::rendering::IndexedTriangleMeshDescription& description) override
		{
			state_->vertices.assign(description.vertices.begin(), description.vertices.end());
			state_->indices.assign(description.indices.begin(), description.indices.end());
			++state_->upload_count;
			return true;
		}

		void Draw(const glm::mat4& model_matrix) const override
		{
			state_->model_matrix = model_matrix;
			++state_->draw_count;
		}

		void Release() noexcept override
		{
			if (released_) return;
			released_ = true;
			++state_->release_count;
		}

	private:
		std::shared_ptr<StaticMeshResourceState> state_;
		bool released_ = false;
	};

	class RecordingStaticMeshBackend final : public ve::rendering::RenderBackend
	{
	public:
		explicit RecordingStaticMeshBackend(
			std::shared_ptr<StaticMeshResourceState> state) noexcept
			: state_(std::move(state)) {}

		[[nodiscard]] ve::rendering::GraphicsApi Api() const noexcept override
		{
			return ve::rendering::GraphicsApi::OpenGLCompatibility;
		}

		[[nodiscard]] ve::rendering::RenderBackendCapabilities Capabilities() const noexcept override
		{
			return {};
		}

		[[nodiscard]] std::unique_ptr<ve::rendering::IndexedTriangleMesh>
		CreateIndexedTriangleMesh(
			const ve::rendering::IndexedTriangleMeshDescription& description) const override
		{
			++state_->creation_attempts;
			if (reject_creation) return nullptr;
			auto resource = std::make_unique<RecordingIndexedTriangleMesh>(state_);
			if (!resource->Upload(description)) return nullptr;
			return resource;
		}

		bool reject_creation = false;

	private:
		std::shared_ptr<StaticMeshResourceState> state_;
	};
}
