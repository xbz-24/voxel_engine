#pragma once

#include <glm/glm.hpp>

namespace ve::rendering
{
	/**
	 * Configures persistent OpenGL state used by world rendering.
	 */
	void ConfigureWorldDefaults();

	/**
	 * Clears the frame using the configured sky color.
	 *
	 * @param red Sky red channel.
	 * @param green Sky green channel.
	 * @param blue Sky blue channel.
	 */
	void BeginWorldFrame(float red, float green, float blue);

	/**
	 * Loads the projection matrix used by the 3D scene.
	 *
	 * @param projection Projection matrix to load into OpenGL.
	 */
	void ApplyProjection(const glm::mat4& projection);

	/**
	 * Loads the view matrix used by the 3D scene.
	 *
	 * @param view Camera view matrix to load into OpenGL.
	 */
	void ApplyView(const glm::mat4& view);

	/**
	 * Uses filled polygons for normal world rendering.
	 */
	void UseSolidFillMode();

	/**
	 * Enables back-face culling for opaque chunk rendering.
	 */
	void UseBackFaceCulling();

	/**
	 * Updates the OpenGL viewport.
	 *
	 * @param width Framebuffer width in pixels.
	 * @param height Framebuffer height in pixels.
	 */
	void SetViewport(int width, int height);
}
