#pragma once

#include <GL/glew.h>

namespace ve::rendering
{
	class GpuOcclusionQuery
	{
	public:
		/** Releases the query object. */
		~GpuOcclusionQuery();

		/** @return True when an OpenGL occlusion query was created. */
		[[nodiscard]] bool Create();

		/** Starts counting samples for the currently rendered proxy geometry. */
		void Begin() const;

		/** Stops counting samples for the current proxy geometry. */
		void End() const;

		/** @return True when the GPU has produced the query result. */
		[[nodiscard]] bool IsReady() const;

		/** @return Number of samples that passed the depth test. */
		[[nodiscard]] GLuint VisibleSampleCount() const;

		/** @param minimum_samples Minimum visible samples required. @return True when enough samples passed. */
		[[nodiscard]] bool WasVisible(GLuint minimum_samples = 0) const;

		/** Deletes the OpenGL query object. */
		void Release();

	private:
		GLuint query_ = 0;
	};
}
