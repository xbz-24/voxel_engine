#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <filesystem>
#include <string>
#include "AssetPaths.h"
#include "Camera.h"
#include "Cube.h"
#include "FrameTimer.h"
#include "Window.h"
#include "SkyBox.h"
#include "Plane.h"
#include "Chunk.h"
#include "World.h"

class Engine
{
public:
	/**
	 * Creates the engine and records the source file path used to resolve assets.
	 */
	Engine();

	/**
	 * Destroys the engine.
	 */
	~Engine();

	/**
	 * Starts the main loop.
	 *
	 * @return 0 when the window closes normally, or -1 when initialization fails.
	 */
	int Run();
private:
	struct HudTextures
	{
		GLuint crosshair;
		GLuint hotbar;
		GLuint experienceBar;
		GLuint heart;
		GLuint hunger;
		GLuint font;
	};

	struct MouseState
	{
		double previousX;
		double previousY;
		bool isFirstInputEvent;
	};

	struct CallbackContext
	{
		Camera* camera;
		MouseState mouse;
	};

	/**
	 * Receives GLFW mouse movement and applies it to the camera.
	 *
	 * @param window Native GLFW window carrying the camera user pointer.
	 * @param xpos Current cursor X position in pixels.
	 * @param ypos Current cursor Y position in pixels.
	 */
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	/**
	 * Configures GLFW callbacks and callback state for engine-owned systems.
	 *
	 * @param window Window whose native callbacks are configured.
	 * @param context Context made reachable from GLFW callbacks.
	 */
	void ConfigureCallbacks(Window& window, CallbackContext& context);

	/**
	 * Configures global OpenGL state used by the current renderer.
	 */
	void ConfigureOpenGLState();

	/**
	 * Loads all textures needed by the HUD.
	 *
	 * @param paths Resolved engine asset paths.
	 * @return Texture id bundle used by RenderHUD.
	 */
	HudTextures LoadHudTextures(const ve::assets::AssetPaths& paths);

	/**
	 * Draws debug X/Y/Z axes in world space.
	 */
	void renderDebugCoordinateSystemAxes();

	/**
	 * Reads keyboard input and moves or closes the window.
	 *
	 * @param window Native GLFW window used for key polling.
	 * @param camera Camera moved by the player controls.
	 * @param frameDeltaTimeSeconds Seconds elapsed since the previous frame.
	 */
	void handlePlayerMovementAndWindowInput(GLFWwindow* window, Camera& camera, double frameDeltaTimeSeconds);

	/**
	 * Casts a ray from the camera to find the block currently targeted.
	 *
	 * @param camera Camera that provides the ray origin and direction.
	 * @param out_hitPos Filled with the hit block coordinates when a hit is found.
	 * @return true when a block is hit, false otherwise.
	 */
	bool performRaycastToFindTargetBlock(Camera& camera, glm::ivec3& out_hitPos);

	/**
	 * Draws a wireframe outline around the selected block.
	 *
	 * @param blockPos Block coordinates to highlight.
	 * @param cube Cube renderer used to draw the outline geometry.
	 */
	void drawBlockHighlight(glm::ivec3 blockPos, Cube& cube);
	std::filesystem::path _applicationSourceFilePath;

	/**
	 * Processes per-frame player input.
	 *
	 * @param window Window wrapper used for native input access.
	 * @param camera Camera controlled by the player.
	 * @param frameTimeDeltaSeconds Seconds elapsed since the previous frame.
	 */
	void ProcessInput(Window& window, Camera& camera, double frameTimeDeltaSeconds);

	/**
	 * Updates simulation state for the current frame.
	 *
	 * @param camera Camera used for selection raycasts.
	 * @param currentlySelectedBlockCoordinates Output selected block coordinates.
	 * @param isBlockSelected Output flag indicating whether a block is selected.
	 */
	void UpdateGameLogic(Camera& camera, glm::ivec3& currentlySelectedBlockCoordinates, bool& isBlockSelected);

	/**
	 * Renders the 3D scene.
	 *
	 * @param window Window used for size-dependent rendering.
	 * @param camera Camera used to build the view matrix.
	 * @param skyBox Skybox renderer.
	 * @param plane Ground plane renderer.
	 * @param cube Cube renderer shared by chunks and selection.
	 * @param world Level chunk container and allocator arena.
	 * @param selectedBlock Currently selected block coordinates.
	 * @param isBlockSelected Whether selectedBlock is valid.
	 */
	void Render3DWorld(const Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, Cube& cube, ve::world::World& world, const glm::ivec3& selectedBlock, bool isBlockSelected);

	/**
	 * Renders the 2D HUD over the 3D scene.
	 *
	 * @param window Window used for HUD positioning.
	 * @param textures Texture ids used by the HUD.
	 * @param fpsText Text drawn in the corner.
	 */
	void RenderHUD(const Window& window, const HudTextures& textures, const std::string& fpsText);

	/**
	 * Rebuilds 3D and 2D projection matrices after window size changes.
	 *
	 * @param width Framebuffer width in pixels.
	 * @param height Framebuffer height in pixels.
	 */
	void UpdateProjections(int width, int height);
	glm::mat4 _projection3D;
	glm::mat4 _projection2D;
	int _currentWindowWidth;
	int _currentWindowHeight;

	/**
	 * Draws a simple procedural cloud layer.
	 */
	void RenderClouds();

	/**
	 * Draws ASCII text using a 16x16 bitmap font texture.
	 *
	 * @param text Text to render.
	 * @param x Left position in screen pixels.
	 * @param y Top position in screen pixels.
	 * @param scale Glyph scale multiplier.
	 * @param fontTexture OpenGL texture id for the font atlas.
	 */
	void RenderText(const std::string& text, float x, float y, float scale, GLuint fontTexture);
};

