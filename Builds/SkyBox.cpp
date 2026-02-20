#include "SkyBox.h"
#include "Utilities.h"
SkyBox::SkyBox(std::string path)
{
    _front = Utils::load_texture((path + "clouds.png").c_str());
    _back = Utils::load_texture((path + "clouds.png").c_str());
    _top = Utils::load_texture((path + "clouds.png").c_str());
    _bottom = Utils::load_texture((path + "clouds.png").c_str());
    _left = Utils::load_texture((path + "clouds.png").c_str());
    _right = Utils::load_texture((path+ "clouds.png").c_str());
}
void SkyBox::draw()
{
    float skySize = 10.0f;
    float textureRepeat = 0.650f;

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Front
    glBindTexture(GL_TEXTURE_2D, _front);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(-skySize, -skySize, -skySize);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(skySize, -skySize, -skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(skySize, skySize, -skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-skySize, skySize, -skySize);
    glEnd();

    // Back  
    glBindTexture(GL_TEXTURE_2D, _back);
    glBegin(GL_QUADS);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(-skySize, -skySize, skySize);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(-skySize, skySize, skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(skySize, skySize, skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(skySize, -skySize, skySize);
    glEnd();

    // Top
    glBindTexture(GL_TEXTURE_2D, _top);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(-skySize, skySize, -skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-skySize, skySize, skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(skySize, skySize, skySize);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(skySize, skySize, -skySize);
    glEnd();

    // Bottom
    glBindTexture(GL_TEXTURE_2D, _bottom);
    glBegin(GL_QUADS);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(-skySize, -skySize, -skySize);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(skySize, -skySize, -skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(skySize, -skySize, skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(-skySize, -skySize, skySize);
    glEnd();

    // Right
    glBindTexture(GL_TEXTURE_2D, _right);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(skySize, -skySize, -skySize);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(skySize, skySize, -skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(skySize, skySize, skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(skySize, -skySize, skySize);
    glEnd();

    // Left
    glBindTexture(GL_TEXTURE_2D, _left);
    glBegin(GL_QUADS);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(-skySize, -skySize, -skySize);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(-skySize, -skySize, skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-skySize, skySize, skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(-skySize, skySize, -skySize);
    glEnd();

    glPopAttrib();
}