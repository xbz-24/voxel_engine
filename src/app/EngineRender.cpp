#include "Engine.h"

#include "Plane.h"
#include "RenderView.h"
#include "RenderState.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

namespace
{
	constexpr float FarWorldClipDistance = 1024.0f;
}

/// Configures persistent OpenGL state used by the engine renderer.
void EngineApplication::ConfigureOpenGLState()
{
	ve::rendering::ConfigureWorldDefaults();
}

/// Renders the visible 3D world and selection highlight.
void EngineApplication::Render3DWorld(ve::engine::RenderView& renderView, const ve::engine::Window&, Camera& camera, SkyBox&, Plane&, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const ve::gameplay::BlockSelection& selection)
{
	ve::rendering::BeginWorldFrame(0.541f, 0.694f, 0.976f);
	ve::rendering::ApplyProjection(_render_cache_state.projection_3d);
	const glm::mat4 view = camera.GetWorldToViewMatrix();
	ve::rendering::ApplyView(view);
	ve::rendering::UseSolidFillMode();
	ve::rendering::UseBackFaceCulling();
	renderView.RenderCloudLayer();
	world.Draw(ve::world::WorldRenderRequest{ blockRegistry, camera.GetPosition(), camera.GetForward(), _render_cache_state.projection_3d * view, _runtimeSettings.renderDistanceChunks });
	renderDebugCoordinateSystemAxes();
	if (selection.has_target)
	{
		drawBlockHighlight(selection.target_block, selectionCube);
	}
}

/// Rebuilds 3D and 2D projection matrices after window size changes.
void EngineApplication::UpdateProjections(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	const float aspect = static_cast<float>(width) / static_cast<float>(height);
	_render_cache_state.projection_3d = glm::frustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, FarWorldClipDistance);
	ve::rendering::SetViewport(width, height);
}
