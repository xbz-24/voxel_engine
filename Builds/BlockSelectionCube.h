#pragma once

#include <GL/glew.h>
#include <string>

class BlockSelectionCube
{
public:
	/**
	 * Loads the block face textures used by the selection preview cube.
	 *
	 * @param textureDirectory Directory containing block texture files.
	 */
	explicit BlockSelectionCube(const std::string& textureDirectory);

	/**
	 * Draws selected cube faces around the current model matrix origin.
	 *
	 * @param halfSize Half-size of the cube in world units.
	 * @param drawFront Whether to draw the front face.
	 * @param drawBack Whether to draw the back face.
	 * @param drawTop Whether to draw the top face.
	 * @param drawBottom Whether to draw the bottom face.
	 * @param drawLeft Whether to draw the left face.
	 * @param drawRight Whether to draw the right face.
	 */
	void Draw(float halfSize, bool drawFront, bool drawBack, bool drawTop, bool drawBottom, bool drawLeft, bool drawRight);

private:
	/// Draws the front face using the loaded side texture.
	void DrawFront(float halfSize) const;
	/// Draws the back face using the loaded side texture.
	void DrawBack(float halfSize) const;
	/// Draws the top face using the loaded grass texture.
	void DrawTop(float halfSize) const;
	/// Draws the bottom face using the loaded dirt texture.
	void DrawBottom(float halfSize) const;
	/// Draws the right face using the loaded side texture.
	void DrawRight(float halfSize) const;
	/// Draws the left face using the loaded side texture.
	void DrawLeft(float halfSize) const;

	GLuint _front = 0;
	GLuint _back = 0;
	GLuint _top = 0;
	GLuint _bottom = 0;
	GLuint _left = 0;
	GLuint _right = 0;
};

