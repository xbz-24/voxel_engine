#include "Engine.h"

#include "Plane.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

void Engine::ConfigureOpenGLState()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
}

void Engine::Render3DWorld(const Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, Cube& cube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const BlockSelection& selection)
{
	glClearColor(0.541f, 0.694f, 0.976f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(_projection3D));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	const glm::mat4 view = camera.GetViewMatrix();
	glLoadMatrixf(glm::value_ptr(view));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	RenderClouds();
	world.Draw(blockRegistry, camera.GetPosition(), camera.GetForward(), _runtimeSettings.renderDistanceChunks);
	renderDebugCoordinateSystemAxes();
	if (selection.hasTarget)
	{
		drawBlockHighlight(selection.targetBlock, cube);
	}
}

void Engine::UpdateProjections(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	const float aspect = static_cast<float>(width) / static_cast<float>(height);
	_projection3D = glm::frustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 100.0f);
	glViewport(0, 0, width, height);
}
