#include <cmath>
#include <filesystem>
#include "AssetPaths.h"
#include "Chunk.h"
#include "Engine.h"
#include "HudRenderer.h"
#include "Logger.h"
#include "Plane.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

Engine::Engine() : _currentWindowHeight(0), _currentWindowWidth(0)
{
	_applicationSourceFilePath = std::filesystem::absolute(__FILE__);
}

Engine::~Engine()
{
}

int Engine::Run()
{
	Window window("Voxel Engine v1.0.0");

	if (window.Initialize() == false)
	{
		return -1;
	}

	Camera camera = Camera();
	CallbackContext callbackContext{ &camera, { 0.0, 0.0, true } };
	ConfigureCallbacks(window, callbackContext);
	ConfigureOpenGLState();

	const ve::assets::AssetPaths assetPaths = ve::assets::ResolveFromSourceFile(_applicationSourceFilePath);

	SkyBox skyBox(assetPaths.environmentTexturesDirectory.string());
	Cube cube(assetPaths.blockTexturesDirectory.string());
	Plane plane((assetPaths.blockTexturesDirectory / "cobblestone.png").string());
	
	const int worldSize = 10;
	const std::size_t expectedChunkCount = static_cast<std::size_t>(worldSize * worldSize);
	ve::world::World world(expectedChunkCount);
	world.SpawnFlatGrid(worldSize);

	ve::ui::HudRenderer hudRenderer(assetPaths);
	ve::time::FrameTimer frameTimer;
	
	bool isBlockSelected = false;
	glm::ivec3 currentlySelectedBlockCoordinates;

	while (!window.ShouldClose())
	{
		if (window.GetWidth() != _currentWindowWidth || window.GetHeight() != _currentWindowHeight)
		{
			_currentWindowWidth = window.GetWidth();
			_currentWindowHeight = window.GetHeight();
			UpdateProjections(_currentWindowWidth, _currentWindowHeight);
		}

		frameTimer.Tick();

		ProcessInput(window, camera, frameTimer.DeltaSeconds());

		UpdateGameLogic(camera, currentlySelectedBlockCoordinates, isBlockSelected);

		Render3DWorld(window, camera, skyBox, plane, cube, world, currentlySelectedBlockCoordinates, isBlockSelected);

		hudRenderer.Draw(window, camera, frameTimer.DisplayedFps(), currentlySelectedBlockCoordinates, isBlockSelected);

		window.Update();
	}
	return 0;
}

void Engine::ConfigureCallbacks(Window& window, CallbackContext& context)
{
	window.SetCallbackUserData(&context);
	glfwSetCursorPosCallback(window.GetNativeWindow(), mouse_callback);
	glfwSetInputMode(window.GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Engine::ConfigureOpenGLState()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
}

void Engine::mouse_callback(GLFWwindow* window, double currentMouseCursorPosX, double currentMouseCursorPosY)
{
	CallbackContext* context = static_cast<CallbackContext*>(Window::GetCallbackUserData(window));

	if (context && context->camera)
	{
		if (context->mouse.isFirstInputEvent)
		{
			context->mouse.previousX = currentMouseCursorPosX;
			context->mouse.previousY = currentMouseCursorPosY;
			context->mouse.isFirstInputEvent = false;
		}

		double mouseMovementDeltaX = currentMouseCursorPosX - context->mouse.previousX;
		double mouseMovementDeltaY = context->mouse.previousY - currentMouseCursorPosY;
		context->mouse.previousX = currentMouseCursorPosX;
		context->mouse.previousY = currentMouseCursorPosY;

		float mouseLookSensitivityCoefficient = 0.1f;
		context->camera->Yaw(mouseMovementDeltaX * mouseLookSensitivityCoefficient);
		context->camera->Pitch(mouseMovementDeltaY * mouseLookSensitivityCoefficient);
	}
	else
	{
		ve::log::Error("Camera pointer is null in mouse callback");
	}
}

void Engine::renderDebugCoordinateSystemAxes()
{
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-10.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -10.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -10.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);

	glEnd();
}

void Engine::handlePlayerMovementAndWindowInput(GLFWwindow* window, Camera& camera, double frameDeltaTimeSeconds)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float playerMovementVelocityScalar = 5.0f * static_cast<float>(frameDeltaTimeSeconds);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.Move(camera.GetForward(), playerMovementVelocityScalar);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.Move(camera.GetForward(), -playerMovementVelocityScalar);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.Move(camera.GetRight(), -playerMovementVelocityScalar);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.Move(camera.GetRight(), playerMovementVelocityScalar);
	}
}	

bool Engine::performRaycastToFindTargetBlock(Camera& camera, glm::ivec3& out_hitPos)
{
	glm::vec3 rayPos = camera.GetPosition();
	glm::vec3 rayDir = camera.GetForward();

	float stepSize = 0.05f;
	float maxReach = 8.0f;

	for (float distance = 0.0f; distance < maxReach; distance += stepSize)
	{
		glm::vec3 currentPos = rayPos + (rayDir * distance);

		int blockX = static_cast<int>(std::floor(currentPos.x));
		int blockY = static_cast<int>(std::floor(currentPos.y));
		int blockZ = static_cast<int>(std::floor(currentPos.z));

		if (blockX == 0 && blockY == 0 && blockZ == 0)
		{
			out_hitPos = glm::ivec3(blockX, blockY, blockZ);
			return true;
		}
	}

	return false;
}

void Engine::drawBlockHighlight(glm::ivec3 blockPos, Cube& cube)
{
	glPushMatrix();

	glTranslatef(blockPos.x, blockPos.y, blockPos.z);

	glTranslatef(0.5f, 0.5f, 0.5f);
	glScalef(1.02f, 1.02f, 1.02f);
	glTranslatef(-0.5f, -0.5f, -0.5f);

	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(7.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	cube.draw(0.5f, true, true, true, true, true, true);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glLineWidth(1.0f);

	glPopMatrix();
}

void Engine::ProcessInput(Window& window, Camera& camera, double frameTimeDeltaSeconds)
{
	handlePlayerMovementAndWindowInput(window.GetNativeWindow(), camera, frameTimeDeltaSeconds);
}

void Engine::UpdateGameLogic(Camera& camera, glm::ivec3& currentlySelectedBlockCoordinates, bool& isBlockSelected)
{
	isBlockSelected = performRaycastToFindTargetBlock(camera, currentlySelectedBlockCoordinates);
}

void Engine::Render3DWorld(const Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, Cube& cube, ve::world::World& world, const glm::ivec3& selectedBlock, bool isBlockSelected)
{
		glClearColor(0.541f, 0.694f, 0.976f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(_projection3D));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glm::mat4 cameraViewTransformationMatrix = camera.GetViewMatrix();
		glLoadMatrixf(glm::value_ptr(cameraViewTransformationMatrix));

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//skyBox.draw();
		//plane.draw();
		//cube.draw();
		RenderClouds();
		world.Draw(cube);

		renderDebugCoordinateSystemAxes();
		
		if (isBlockSelected)
		{
			drawBlockHighlight(selectedBlock, cube);
		}
}

void Engine::UpdateProjections(int width, int height)
{
	if (height == 0) height = 1; 

	float aspect = static_cast<float>(width) / static_cast<float>(height);

	_projection3D = glm::frustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 100.f);

	glViewport(0, 0, width, height);
}
void Engine::RenderClouds()
{
	
	glDisable(GL_TEXTURE_2D);

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	glColor4f(1.0f, 1.0f, 1.0f, 0.8f);

	glBegin(GL_QUADS);

	float cloudHeight = 120.0f; 
	float cloudSize = 8.0f;     

	
	for (float x = -200.0f; x < 200.0f; x += cloudSize)
	{
		for (float z = -200.0f; z < 200.0f; z += cloudSize)
		{
			
			float cloudShape = std::sin(x * 0.05f) * std::cos(z * 0.05f);

			
			if (cloudShape > 0.2f)
			{
				glVertex3f(x, cloudHeight, z);
				glVertex3f(x + cloudSize, cloudHeight, z);
				glVertex3f(x + cloudSize, cloudHeight, z + cloudSize);
				glVertex3f(x, cloudHeight, z + cloudSize);
			}
		}
	}

	glEnd();

	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}
