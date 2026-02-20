#include "Cube.h"
#include "Utilities.h"

Cube::Cube(std::string p){
    _front  = Utils::load_texture((p + "grass_block_side.png").c_str());
    _back   = Utils::load_texture((p + "grass_block_side.png").c_str());
    _top    = Utils::load_texture((p + "grass_block_side.png").c_str());
    _bottom = Utils::load_texture((p + "grass_block_side.png").c_str());
    _left   = Utils::load_texture((p + "grass_block_side.png").c_str());
    _right  = Utils::load_texture((p + "grass_block_side.png").c_str());
}
void Cube::draw(float cubeSize) {
    float s = cubeSize;

    glBindTexture(GL_TEXTURE_2D, _front);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, _back);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, -s, -s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, _top);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, s, -s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, _bottom);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, -s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, -s, s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, _right);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, -s, s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, _left);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, -s);
    glEnd();
}