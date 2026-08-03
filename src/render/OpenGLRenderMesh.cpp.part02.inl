					const void* first_index_offset = OpenGLIndexByteOffset(batch.first_index);
					glDrawElements(GL_TRIANGLES, OpenGLCount(batch.index_count), GL_UNSIGNED_INT, first_index_offset);
				}

				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glColor3f(1.0f, 1.0f, 1.0f);
			}

			void Release() override
			{
				if (vertex_buffer_ != 0)
				{
					glDeleteBuffers(1, &vertex_buffer_);
					vertex_buffer_ = 0;
				}
				if (index_buffer_ != 0)
				{
					glDeleteBuffers(1, &index_buffer_);
					index_buffer_ = 0;
				}
				vertex_count_ = 0;
				index_count_ = 0;
				indexed_batches_.clear();
			}

		private:
			struct IndexedChunkMeshBatch
			{
				TextureHandle texture = kInvalidTextureHandle;
				std::uint32_t first_index = 0;
				std::uint32_t index_count = 0;
			};

			GLuint vertex_buffer_ = 0;
			GLuint index_buffer_ = 0;
			GLsizei vertex_count_ = 0;
			GLsizei index_count_ = 0;
			std::vector<IndexedChunkMeshBatch> indexed_batches_;
		};
	}

	std::unique_ptr<RenderMesh> CreateOpenGLRenderMesh()
	{
		return std::make_unique<OpenGLRenderMesh>();
	}
}
