	}

	const ChunkFaceGeometry& GeometryForDirection(MeshFaceDirection direction) noexcept
	{
		for (const ChunkFaceGeometry& geometry : ChunkFaceGeometries())
		{
			if (geometry.direction == direction)
			{
				return geometry;
			}
		}
		return ChunkFaceGeometries().front();
	}
}
