#include "OpenGLRenderMesh.h"

#include "OpenGLTypeConversions.h"
#include "TextureLoader.h"

#include <cstddef>

namespace ve::rendering
{
	void OpenGLRenderMesh::Draw() const
	{
		if (vertex_buffer_ == 0 || index_buffer_ == 0 || vertex_count_ == 0 || index_count_ == 0)
		{
			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(ChunkVertex),
			reinterpret_cast<void*>(offsetof(ChunkVertex, x)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(ChunkVertex),
			reinterpret_cast<void*>(offsetof(ChunkVertex, u)));
		glColorPointer(3, GL_FLOAT, sizeof(ChunkVertex),
			reinterpret_cast<void*>(offsetof(ChunkVertex, r)));

		for (const IndexedChunkMeshBatch& batch : indexed_batches_)
		{
			glBindTexture(GL_TEXTURE_2D, NativeOpenGLTexture(batch.texture));
			const void* first_index_offset = OpenGLIndexByteOffset(batch.first_index);
			glDrawElements(GL_TRIANGLES, OpenGLCount(batch.index_count),
				GL_UNSIGNED_INT, first_index_offset);
		}

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glColor3f(1.0f, 1.0f, 1.0f);
	}
}
