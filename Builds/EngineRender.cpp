#include "Engine.h"

#include "Plane.h"
#include "RenderState.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

/// Configures persistent OpenGL state used by the engine renderer.
void Engine::ConfigureOpenGLState()
{
	ve::rendering::ConfigureWorldDefaults();
}

/// Renders the visible 3D world and selection highlight.
void Engine::Render3DWorld(const Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, Cube& cube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const BlockSelection& selection)
{
	ve::rendering::BeginWorldFrame(0.541f, 0.694f, 0.976f);
	ve::rendering::ApplyProjection(_projection3D);
	const glm::mat4 view = camera.GetViewMatrix();
	ve::rendering::ApplyView(view);
	ve::rendering::UseSolidFillMode();
	RenderClouds();
	world.Draw(blockRegistry, camera.GetPosition(), camera.GetForward(), _runtimeSettings.renderDistanceChunks);
	renderDebugCoordinateSystemAxes();
	if (selection.hasTarget)
	{
		drawBlockHighlight(selection.targetBlock, cube);
	}
}

/// Rebuilds 3D and 2D projection matrices after window size changes.
void Engine::UpdateProjections(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	const float aspect = static_cast<float>(width) / static_cast<float>(height);
	_projection3D = glm::frustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 100.0f);
	ve::rendering::SetViewport(width, height);
}
