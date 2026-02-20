#pragma once
#include <GL/glew.h>
#include <string>
class SkyBox
{
private:
	GLuint _front, _back, _top, _bottom, _left, _right;
public:
	SkyBox(std::string path);
	void draw();
};

