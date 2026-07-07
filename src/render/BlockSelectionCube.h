#pragma once

#include "GraphicsTypes.h"

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
	 * @param drawFront True when the front face should be drawn.
	 * @param drawBack True when the back face should be drawn.
	 * @param drawTop True when the top face should be drawn.
	 * @param drawBottom True when the bottom face should be drawn.
	 * @param drawLeft True when the left face should be drawn.
	 * @param drawRight True when the right face should be drawn.
	 */
	void Draw(float halfSize, bool drawFront, bool drawBack, bool drawTop, bool drawBottom, bool drawLeft, bool drawRight);

private:
	/// Draws the front face with the side texture.
	void DrawFront(float halfSize) const;
	/// Draws the back face with the side texture.
	void DrawBack(float halfSize) const;
	/// Draws the top face with the grass texture.
	void DrawTop(float halfSize) const;
	/// Draws the bottom face with the dirt texture.
	void DrawBottom(float halfSize) const;
	/// Draws the right face with the side texture.
	void DrawRight(float halfSize) const;
	/// Draws the left face with the side texture.
	void DrawLeft(float halfSize) const;

	ve::rendering::TextureHandle _front;
	ve::rendering::TextureHandle _back;
	ve::rendering::TextureHandle _top;
	ve::rendering::TextureHandle _bottom;
	ve::rendering::TextureHandle _left;
	ve::rendering::TextureHandle _right;
};

