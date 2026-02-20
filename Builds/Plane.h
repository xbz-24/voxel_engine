#pragma once
#include <GL/glew.h>
#include <string>
class Plane
{
private: 
	GLuint _texture;
public:
	Plane(std::string path);
	void draw();
};

