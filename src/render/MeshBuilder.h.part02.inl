		/** @param first First triangle point. @param second Second point. @param third Third point. @return Unit face normal. */
		[[nodiscard]] static glm::vec3 FaceNormal(glm::vec3 first, glm::vec3 second, glm::vec3 third)
		{
			const glm::vec3 cross_product = glm::cross(second - first, third - first);
			return glm::length(cross_product) > 0.0f ? glm::normalize(cross_product) : glm::vec3{ 0.0f, 1.0f, 0.0f };
		}

		Mesh mesh_;
	};

	using MeshBuilder = BasicMeshBuilder<>;
}

#include "MeshBuilderCube.inl"
