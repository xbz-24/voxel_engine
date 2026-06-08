#include "Engine.h"

#include <cmath>

void Engine::RenderClouds()
{
	if (_cloudDisplayListID == 0)
	{
		BuildCloudDisplayList();
	}
	glCallList(_cloudDisplayListID);
}

void Engine::BuildCloudDisplayList()
{
	_cloudDisplayListID = glGenLists(1);
	glNewList(_cloudDisplayListID, GL_COMPILE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	glBegin(GL_QUADS);

	constexpr float cloudHeight = 120.0f;
	constexpr float cloudSize = 8.0f;
	for (float x = -200.0f; x < 200.0f; x += cloudSize)
	{
		for (float z = -200.0f; z < 200.0f; z += cloudSize)
		{
			const float cloudShape = std::sin(x * 0.05f) * std::cos(z * 0.05f);
			if (cloudShape > 0.2f)
			{
				glVertex3f(x, cloudHeight, z);
				glVertex3f(x + cloudSize, cloudHeight, z);
				glVertex3f(x + cloudSize, cloudHeight, z + cloudSize);
				glVertex3f(x, cloudHeight, z + cloudSize);
			}
		}
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEndList();
}
