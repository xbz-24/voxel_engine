#pragma once

#include <GL/glew.h>
#include <string>

class SkyBox
{
private:
	GLuint _front, _back, _top, _bottom, _left, _right;
public:
	/**
	 * Loads the skybox textures.
	 *
	 * @param path Directory containing skybox texture files.
	 */
	explicit SkyBox(const std::string& path);

	/**
	 * Draws the skybox around the current origin.
	 */
	void draw();
};

