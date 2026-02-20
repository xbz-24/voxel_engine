#include <filesystem>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "Builds/Camera.h"
#include "Builds/Cube.h"
#include "Builds/Plane.h"
#include "Builds/SkyBox.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawAxes();

int main() {

	if (!glfwInit()) {
		return -1;
	}

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Opengl sandbox", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	Camera camera(0.0f, 0.0f, 3.0f, -90.0f, 0.0f, 0.05f, 0.1f);

	glfwSetWindowUserPointer(window, &camera);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	std::filesystem::path mainFile = __FILE__;
	std::filesystem::path rootDir = mainFile.parent_path();
	std::string p = (rootDir / "assets/textures/block/").string();
	std::string skyPath = (rootDir / "assets/textures/environment/").string();

	SkyBox skyBox(skyPath);
	Cube cube(p);
	Plane plane(p + "cobblestone.png");

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		float aspect = 800.0f / 600.0f;
		glFrustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 100.0f);

		camera.apply_to_scene();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		skyBox.draw();
		plane.draw();
		cube.draw();
		drawAxes();

		glfwSwapBuffers(window);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.move_forward();
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.move_backward();
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.move_left();
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.move_right();
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.move_down();
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.move_up();
		}
		glfwPollEvents();
	}
}


void drawAxes() {
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	Camera* camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));

	if (camera) {
		static double lastX = 400, lastY = 300;
		static bool firstMouse = true;

		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		double xoffset = xpos - lastX;
		double yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		camera->rotate(xoffset, yoffset);
	}
	else {
		std::cerr << std::endl << "Camera pointer is null in mouse callback!" << std::endl;
	}
}
