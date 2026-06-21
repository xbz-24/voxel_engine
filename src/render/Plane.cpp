#include "Plane.h"
#include "TextureLoader.h"

Plane::Plane(const std::string& path) 
{
	_texture = ve::rendering::LoadTexture(path.c_str());
}

void Plane::draw() 
{
    glBindTexture(GL_TEXTURE_2D, _texture);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);

    float tileAmount = 10.0f;

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -0.5f, -10.0f);
    glTexCoord2f(tileAmount, 0.0f); glVertex3f(10.0f, -0.5f, -10.0f);
    glTexCoord2f(tileAmount, tileAmount); glVertex3f(10.0f, -0.5f, 10.0f);
    glTexCoord2f(0.0f, tileAmount); glVertex3f(-10.0f, -0.5f, 10.0f);

    glEnd();
}
