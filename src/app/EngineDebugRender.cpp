#include "Engine.h"

void Engine::renderDebugCoordinateSystemAxes()
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-10.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -10.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -10.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
}

void Engine::drawBlockHighlight(glm::ivec3 blockPos, BlockSelectionCube& selectionCube)
{
	glPushMatrix();
	glTranslatef(static_cast<float>(blockPos.x), static_cast<float>(blockPos.y), static_cast<float>(blockPos.z));
	glTranslatef(0.5f, 0.5f, 0.5f);
	glScalef(1.02f, 1.02f, 1.02f);
	glTranslatef(-0.5f, -0.5f, -0.5f);

	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(7.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	selectionCube.Draw(0.5f, true, true, true, true, true, true);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glLineWidth(1.0f);
	glPopMatrix();
}
