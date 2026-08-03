	{
		return UploadOpenGLTexture(DecodeImageFile(path));
	}

	/**
	 * Sets the current immediate-mode OpenGL color from a packed RGB value.
	 *
	 * @param hexColor RGB color encoded as 0xRRGGBB.
	 */
	void SetColorFromHex(int hexColor)
	{
		const float red = PackedRgbUnitChannel(hexColor, 16);
		const float green = PackedRgbUnitChannel(hexColor, 8);
		const float blue = PackedRgbUnitChannel(hexColor, 0);
		glColor3f(red, green, blue);
	}
}
