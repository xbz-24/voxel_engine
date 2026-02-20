#pragma once
#include <GL/glew.h>
#include <string>

class Cube
{
private:
	GLuint _front, _back, _top, _bottom, _left, _right;
public:
	Cube(std::string path);
	void draw(float cubeSize = 0.5f);
};

