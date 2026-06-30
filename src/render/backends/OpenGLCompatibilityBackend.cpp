#include "OpenGLCompatibilityBackend.h"

#include <GL/glew.h>

namespace ve::rendering
{
	/** Returns conservative capabilities for the legacy OpenGL path. */
	RenderBackendCapabilities OpenGLCompatibilityBackend::Capabilities() const noexcept
	{
		return { false, false, false, true };
	}

	/** Returns the compatibility API identifier. */
	GraphicsApi OpenGLCompatibilityBackend::Api() const noexcept
	{
		return GraphicsApi::OpenGLCompatibility;
	}

	std::unique_ptr<RenderMesh> OpenGLCompatibilityBackend::CreateMeshResource() const
	{
		return CreateOpenGLRenderMesh();
	}

	const ComputeDispatcher* OpenGLCompatibilityBackend::Compute() const noexcept
	{
		return &compute_dispatcher_;
	}

	bool OpenGLCompatibilityBackend::OpenGLComputeDispatcher::IsSupported() const noexcept
	{
		return GLEW_VERSION_4_3 || GLEW_ARB_compute_shader;
	}

	void OpenGLCompatibilityBackend::OpenGLComputeDispatcher::Dispatch(ComputeProgramHandle program, ComputeDispatchSize size) const
	{
		if (!IsSupported() || !program.IsValid()) return;
		glUseProgram(program.value);
		glDispatchCompute(size.groups_x, size.groups_y, size.groups_z);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glUseProgram(0);
	}
}
