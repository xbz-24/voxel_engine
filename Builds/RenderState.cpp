#include "RenderState.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace ve::rendering
{
	/// Configures persistent OpenGL state used by world rendering.
	void ConfigureWorldDefaults()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
	}

	/// Clears the frame using the configured sky color.
	void BeginWorldFrame(float red, float green, float blue)
	{
		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	/// Loads the projection matrix used by the 3D scene.
	void ApplyProjection(const glm::mat4& projection)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(projection));
	}

	/// Loads the view matrix used by the 3D scene.
	void ApplyView(const glm::mat4& view)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(view));
	}

	/// Uses filled polygons for normal world rendering.
	void UseSolidFillMode()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	/// Enables back-face culling for opaque chunk rendering.
	void UseBackFaceCulling()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}

	/// Updates the OpenGL viewport.
	void SetViewport(int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}
