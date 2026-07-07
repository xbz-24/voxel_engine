#pragma once

#include "GraphicsTypes.h"

#include <string>

class SkyBox
{
private:
	ve::rendering::TextureHandle _front;
	ve::rendering::TextureHandle _back;
	ve::rendering::TextureHandle _top;
	ve::rendering::TextureHandle _bottom;
	ve::rendering::TextureHandle _left;
	ve::rendering::TextureHandle _right;
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

