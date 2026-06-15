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
void Engine::Render3DWorld(const Window&, Camera& camera, SkyBox&, Plane&, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const ve::gameplay::BlockSelection& selection)
{
	ve::rendering::BeginWorldFrame(0.541f, 0.694f, 0.976f);
	ve::rendering::ApplyProjection(_render_cache_state.projection_3d);
	const glm::mat4 view = camera.GetViewMatrix();
	ve::rendering::ApplyView(view);
	ve::rendering::UseSolidFillMode();
	ve::rendering::UseBackFaceCulling();
	RenderClouds();
	world.Draw(ve::world::WorldRenderRequest{ blockRegistry, camera.GetPosition(), camera.GetForward(), _render_cache_state.projection_3d * view, _runtimeSettings.renderDistanceChunks });
	renderDebugCoordinateSystemAxes();
	if (selection.has_target)
	{
		drawBlockHighlight(selection.target_block, selectionCube);
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
	_render_cache_state.projection_3d = glm::frustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 100.0f);
	ve::rendering::SetViewport(width, height);
}
