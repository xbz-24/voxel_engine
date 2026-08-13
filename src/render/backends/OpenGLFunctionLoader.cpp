#include "OpenGLFunctionLoader.h"

#include "Logger.h"

#include <GL/glew.h>

namespace ve::rendering
{
	bool InitializeOpenGLFunctionLoader() noexcept
	{
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			VE_LOG_ERROR("Failed to initialize the OpenGL function loader");
			return false;
		}
		// GLEW may leave GL_INVALID_ENUM behind when probing a core profile.
		(void)glGetError();
		return true;
	}
}
