#pragma once

#include "GraphicsTypes.h"

#include <string>

class Plane
{
private: 
	ve::rendering::TextureHandle _texture;
public:
	/**
	 * Loads the texture used by the plane.
	 *
	 * @param path Texture file path.
	 */
	explicit Plane(const std::string& path);

	/**
	 * Draws a textured ground plane.
	 */
	void draw();
};

