#include "GpuOcclusionQuery.h"

namespace ve::rendering
{
	/// Releases the query object.
	GpuOcclusionQuery::~GpuOcclusionQuery()
	{
		Release();
	}

	/// Creates an OpenGL occlusion query.
	bool GpuOcclusionQuery::Create()
	{
		Release();
		glGenQueries(1, &query_);
		return query_ != 0;
	}

	/// Starts counting visible samples for proxy geometry.
	void GpuOcclusionQuery::Begin() const
	{
		if (query_ != 0) glBeginQuery(GL_SAMPLES_PASSED, query_);
	}

	/// Stops counting visible samples for proxy geometry.
	void GpuOcclusionQuery::End() const
	{
		if (query_ != 0) glEndQuery(GL_SAMPLES_PASSED);
	}

	/// Reports whether the GPU has produced the query result.
	bool GpuOcclusionQuery::IsReady() const
	{
		if (query_ == 0) return false;
		GLint is_ready = GL_FALSE;
		glGetQueryObjectiv(query_, GL_QUERY_RESULT_AVAILABLE, &is_ready);
		return is_ready == GL_TRUE;
	}

	/// Returns the number of samples that passed the depth test.
	GLuint GpuOcclusionQuery::VisibleSampleCount() const
	{
		GLuint sample_count = 0;
		if (query_ != 0) glGetQueryObjectuiv(query_, GL_QUERY_RESULT, &sample_count);
		return sample_count;
	}

	/// Checks whether enough samples were visible.
	bool GpuOcclusionQuery::WasVisible(GLuint minimum_samples) const
	{
		return IsReady() && VisibleSampleCount() > minimum_samples;
	}

	/// Deletes the OpenGL query object.
	void GpuOcclusionQuery::Release()
	{
		if (query_ != 0) glDeleteQueries(1, &query_);
		query_ = 0;
	}
}
