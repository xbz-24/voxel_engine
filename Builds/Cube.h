#pragma once

#include <GL/glew.h>
#include <string>

class Cube
{
private:
	GLuint _front, _back, _top, _bottom, _left, _right;
public:
	/**
	 * Loads the block face textures used by cube rendering.
	 *
	 * @param path Directory containing block texture files.
	 */
	explicit Cube(const std::string& path);

	/**
	 * Draws selected cube faces around the current model matrix origin.
	 *
	 * @param cubeSize Half-size of the cube in world units.
	 * @param drawFront Whether to draw the front face.
	 * @param drawBack Whether to draw the back face.
	 * @param drawTop Whether to draw the top face.
	 * @param drawBottom Whether to draw the bottom face.
	 * @param drawLeft Whether to draw the left face.
	 * @param drawRight Whether to draw the right face.
	 */
	void draw(float cubeSize, bool drawFront, bool drawBack, bool drawTop, bool drawBottom, bool drawLeft, bool drawRight);

	/**
	 * Returns the front face texture id.
	 *
	 * @return OpenGL texture id.
	 */
	GLuint GetFront() const { return _front; }

	/**
	 * Returns the back face texture id.
	 *
	 * @return OpenGL texture id.
	 */
	GLuint GetBack() const { return _back; }

	/**
	 * Returns the top face texture id.
	 *
	 * @return OpenGL texture id.
	 */
	GLuint GetTop() const { return _top; }

	/**
	 * Returns the bottom face texture id.
	 *
	 * @return OpenGL texture id.
	 */
	GLuint GetBottom() const { return _bottom; }

	/**
	 * Returns the left face texture id.
	 *
	 * @return OpenGL texture id.
	 */
	GLuint GetLeft() const { return _left; }

	/**
	 * Returns the right face texture id.
	 *
	 * @return OpenGL texture id.
	 */
	GLuint GetRight() const { return _right; }
};

