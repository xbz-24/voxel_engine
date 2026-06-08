#include "Cube.h"
#include "Utilities.h"

Cube::Cube(const std::string& path)
{
    _front  = Utils::load_texture((path + "grass_block_side.png").c_str());
    _back   = Utils::load_texture((path + "grass_block_side.png").c_str());
    _left = Utils::load_texture((path + "grass_block_side.png").c_str());
    _right = Utils::load_texture((path + "grass_block_side.png").c_str());

    _top = Utils::load_texture((path + "grass_block_top.png").c_str());
    _bottom = Utils::load_texture((path + "dirt.png").c_str());

}
void Cube::draw(float cubeSize, bool drawFront, bool drawBack, bool drawTop, bool drawBottom, bool drawLeft, bool drawRight)
{
	float s = cubeSize;

	if (drawFront)
	{
		glBindTexture(GL_TEXTURE_2D, _front);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, s);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, s);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, s);
		glEnd();
	}

	if (drawBack)
	{
		glBindTexture(GL_TEXTURE_2D, _back);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, -s);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, s, -s);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(s, s, -s);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, -s);
		glEnd();
	}

	if (drawTop)
	{
		glBindTexture(GL_TEXTURE_2D, _top);
		glColor3f(0.404f, 0.655f, 0.239f); // Your exact hex color!
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, s, -s);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, s);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(s, s, -s);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f); // Reset
	}

	if (drawBottom)
	{
		glBindTexture(GL_TEXTURE_2D, _bottom);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, -s);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(s, -s, s);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, -s, s);
		glEnd();
	}

	if (drawRight)
	{
		glBindTexture(GL_TEXTURE_2D, _right);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, -s);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, -s);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(s, s, s);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, s);
		glEnd();
	}

	if (drawLeft)
	{
		glBindTexture(GL_TEXTURE_2D, _left);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, s);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, s, s);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, -s);
		glEnd();
	}
}
