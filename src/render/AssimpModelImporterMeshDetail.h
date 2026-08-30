#pragma once

#include "ImportedModelMesh.h"

struct aiMesh;

namespace ve::assets::detail
{
	[[nodiscard]] ImportedVertex ReadVertex(const aiMesh& mesh, unsigned int vertex_index);
	void ReadJoints(const aiMesh& mesh, ImportedMesh& imported_mesh);
}
