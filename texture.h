//texture.h
#ifndef __TEXTURE_H
#define __TEXTURE_H

#include "main.h"

#define BITMAP_ID 0x4D42	// the universal bitmap ID

typedef struct{
	GLubyte Header[12];									// TGA File Header
} TGAHeader;


class CTexture{
public:
	//bool Load(char *filename);
	void Bind();

	CTexture(const char *filename, GLenum filter);
	CTexture(int width, int height, int channels, GLenum filter);
	bool SaveTGA(const char *filename);
	~CTexture();
	void LoadBMP(const char *filename);
	void LoadTGA(const char *filename);
	GLuint width,height,channels;
	GLint format;		//GL_RGB or GL_RGBA
	GLenum type;		//GL_FLOAT or GL_UNSIGNED_BYTE
	GLenum target;
	GLuint texID;
	GLint internalFormat;
private:
	unsigned char *data;
};


#endif