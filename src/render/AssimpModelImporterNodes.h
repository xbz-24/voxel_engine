#pragma once

#include "ImportedModelMesh.h"

struct aiNode;

namespace ve::assets::detail
{
	[[nodiscard]] ImportedNode ReadSceneNode(const aiNode& source_node);
}
