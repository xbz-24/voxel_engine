#include "BlockSelectionCube.h"

#include "TextureLoader.h"

/**
 * Loads the block face textures used by the selection preview cube.
 *
 * @param textureDirectory Directory containing block texture files.
 */
BlockSelectionCube::BlockSelectionCube(const std::string& textureDirectory)
{
    _front  = ve::rendering::LoadTexture((textureDirectory + "grass_block_side.png").c_str());
    _back   = ve::rendering::LoadTexture((textureDirectory + "grass_block_side.png").c_str());
    _left = ve::rendering::LoadTexture((textureDirectory + "grass_block_side.png").c_str());
    _right = ve::rendering::LoadTexture((textureDirectory + "grass_block_side.png").c_str());

    _top = ve::rendering::LoadTexture((textureDirectory + "grass_block_top.png").c_str());
    _bottom = ve::rendering::LoadTexture((textureDirectory + "dirt.png").c_str());

}
/**
 * Draws selected cube faces around the current model matrix origin.
 *
 * @param halfSize Half-size of the cube in world units.
 * @param drawFront True when the front face should be emitted.
 * @param drawBack True when the back face should be emitted.
 * @param drawTop True when the top face should be emitted.
 * @param drawBottom True when the bottom face should be emitted.
 * @param drawLeft True when the left face should be emitted.
 * @param drawRight True when the right face should be emitted.
 */
void BlockSelectionCube::Draw(float halfSize, bool drawFront, bool drawBack, bool drawTop, bool drawBottom, bool drawLeft, bool drawRight)
{
	if (drawFront)
	{
		DrawFront(halfSize);
	}
	if (drawBack)
	{
		DrawBack(halfSize);
	}
	if (drawTop)
	{
		DrawTop(halfSize);
	}
	if (drawBottom)
	{
		DrawBottom(halfSize);
	}
	if (drawRight)
	{
		DrawRight(halfSize);
	}
	if (drawLeft)
	{
		DrawLeft(halfSize);
	}
}
