//texture.cpp
#include "texture.h"
#include "glWindow.h"

extern CglWindow* g_win;


CTexture::CTexture(const char *filename, GLenum filter){
	memset(this,0,sizeof(*this));
	if(!filename) 
		return;
	if(strstr(filename, ".bmp") || strstr(filename, ".BMP"))
		LoadBMP(filename);
	else if(strstr(filename, ".tga") || strstr(filename, ".TGA"))
		LoadTGA(filename);
	else{
		MessageBox(g_win->hwnd, "Unsupported texture type", "Error", MB_OK);
	}
	target=GL_TEXTURE_RECTANGLE_ARB;
	glGenTextures(1, &texID);
	glBindTexture(target, texID);
	type=GL_UNSIGNED_BYTE;
	if(channels==3){
		internalFormat=GL_RGB;
		format=GL_BGR;	//BGR to avoid manual swapping of R and B channels, and avoid swizzling on Ati cards when using FBO
	}
	else{
		internalFormat=GL_RGBA;
		format=GL_BGRA;
	}
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,filter);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	glBindTexture(target, 0);
}


//used to create render target texture
CTexture::CTexture(int width, int height, int channels, GLenum filter){
	memset(this,0,sizeof(*this));
	this->height=height;
	this->width=width;
	this->channels=channels;
	target=GL_TEXTURE_RECTANGLE_ARB;
	type=GL_UNSIGNED_BYTE;
	if(channels==3){
		internalFormat=GL_RGB;
		format=GL_BGR;
	}
	else{
		internalFormat=GL_RGBA;
		format=GL_BGRA;
	}
	glGenTextures(1, &texID);
	glBindTexture(target, texID);

	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, 0);

	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,filter);
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,filter);
	glBindTexture(target, 0);
}



CTexture::~CTexture(){
	if(data){
		delete[] data;
		data=0;
	}
	if(texID){
		glDeleteTextures(1, &texID);
	}
}



void CTexture::Bind(){
	glBindTexture(target, texID);
}


void CTexture::LoadBMP(const char *filename){
	FILE *filePtr;
	BITMAPINFOHEADER	bitmapInfoHeader;
	BITMAPFILEHEADER	bitmapFileHeader;
	unsigned int		imageIdx = 0;			// image index counter
	//unsigned char		tempRGB;				// swap variable

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL){
		MessageBox(g_win->hwnd, "Texture not found", "Error", MB_OK);
		return;
	}

	// read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	
	// verify that this is a bitmap by checking for the universal bitmap id
	if (bitmapFileHeader.bfType != BITMAP_ID){
		MessageBox(g_win->hwnd, "Invalid bitmap type", "Error", MB_OK);
		fclose(filePtr);
		return;
	}

	// read the bitmap information header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	if(bitmapInfoHeader.biSizeImage == 0){
		bitmapInfoHeader.biSizeImage = ((((bitmapInfoHeader.biWidth * bitmapInfoHeader.biBitCount) + 31) & ~31) / 8)
						* bitmapInfoHeader.biHeight;
	}

	// move file pointer to beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	data=(unsigned char*)malloc(bitmapInfoHeader.biSizeImage);

	// verify memory allocation
	if (!data){
		MessageBox(g_win->hwnd, "Could not allocate bitmap memory", "Error", MB_OK);
		delete[] data;
		fclose(filePtr);
		return;
	}

	// read in the bitmap image data
	fread(data, 1, bitmapInfoHeader.biSizeImage, filePtr);

	// make sure bitmap image data was read
	if (data == NULL){
		MessageBox(g_win->hwnd, "Error reading bitmap data", "Error", MB_OK);
		fclose(filePtr);
		return;
	}

	// swap the R and B values to get RGB since the bitmap color format is in BGR
//	for (imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx+=3){
//		tempRGB = data[imageIdx];
//		data[imageIdx] = data[imageIdx + 2];
//		data[imageIdx + 2] = tempRGB;
//	}

	// close the file and return the bitmap image data
	fclose(filePtr);

	// Assign the channels, width, height to pImage
	channels = 3;
	width = bitmapInfoHeader.biWidth;
	height = bitmapInfoHeader.biHeight;

}


void CTexture::LoadTGA(const char *filename){
	static GLubyte uTGAcompare[12] = {0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
	static GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header
	FILE *filePtr;
	TGAHeader tgaheader;
	GLubyte header[6];
	GLuint imageSize;
	unsigned int imageIdx = 0;			// image index counter

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL){
		return;
	}

	// read the file header
	if(fread(&tgaheader, sizeof(TGAHeader), 1, filePtr) == 0){
		if(filePtr != NULL)
			fclose(filePtr);
		return;
	}

	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0){												
		//LoadUncompressedTGA
		fread(header, sizeof(header), 1, filePtr);
		width = header[1] * 256 + header[0];
		height = header[3] * 256 + header[2];
		channels = header[4]/8;
		imageSize = width*height*channels;
		data = new GLubyte[imageSize];

		// verify memory allocation
		if (!data){
			delete[] data;
			fclose(filePtr);
			return;
		}
		fread(data, 1, imageSize, filePtr);
		// make sure bitmap image data was read
		if (data == NULL){
			fclose(filePtr);
			return;
		}
		// Byte Swapping
//		for(imageIdx = 0; imageIdx < (int)imageSize; imageIdx += channels){
//			data[imageIdx] ^= data[imageIdx+2] ^= data[imageIdx] ^= data[imageIdx+2];
//		}

		fclose(filePtr);
		return;

	}
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0){																	
		//LoadCompressedTGA
		fread(header, sizeof(header), 1, filePtr);
		width = header[1] * 256 + header[0];
		height = header[3] * 256 + header[2];
		channels = header[4]/8;
		imageSize = width*height*channels;
		data = new GLubyte[imageSize];
		// verify memory allocation
		if (!data){
			delete[] data;
			fclose(filePtr);
			return;
		}
		GLuint pixelcount = width * height;
		GLuint currentpixel	= 0;
		GLuint currentbyte	= 0;
		GLubyte* colorbuffer = new GLubyte[channels];
		do{
			GLubyte chunkheader = 0;
			if(fread(&chunkheader, sizeof(GLubyte), 1, filePtr) == 0){
				if(filePtr != NULL){
					fclose(filePtr);
				}
				if(data != NULL){
					delete[] data;
				}
				return;
			}
			if(chunkheader < 128){
				++chunkheader;
				for(int counter = 0; counter < chunkheader; ++counter){
					if(fread(colorbuffer, 1, channels, filePtr) != channels){
						if(filePtr != NULL){
							fclose(filePtr);
						}
						if(colorbuffer != NULL){
							delete[] colorbuffer;
						}
						if(data != NULL){
							delete[] data;
						}
						return;
					}
					data[currentbyte] = colorbuffer[0];
					data[currentbyte+1] = colorbuffer[1];
					data[currentbyte+2] = colorbuffer[2];
					if(channels==4){
						data[currentbyte+3] = colorbuffer[3];
					}
					currentbyte += channels;
					++currentpixel;
					if(currentpixel > pixelcount){
						//too many pixels read
						if(filePtr != NULL){
							fclose(filePtr);
						}
						if(colorbuffer != NULL){
							delete[] colorbuffer;
						}
						if(data != NULL){
							delete[] data;
						}
						return;
					}
				}
			}
			else{
				chunkheader -= 127;
				if(fread(colorbuffer, 1, channels, filePtr) != channels){
					if(filePtr != NULL){
						fclose(filePtr);
					}
					if(colorbuffer != NULL){
						delete[] colorbuffer;
					}
					if(data != NULL){
						delete[] data;
					}
					return;
				}
				for(int counter = 0; counter < chunkheader; ++counter){
					data[currentbyte] = colorbuffer[0];
					data[currentbyte+1] = colorbuffer[1];
					data[currentbyte+2] = colorbuffer[2];
					if(channels==4){
						data[currentbyte+3] = colorbuffer[3];
					}
					currentbyte += channels;
					++currentpixel;
					if(currentpixel > pixelcount){
						//too many pixels read
						if(filePtr != NULL){
							fclose(filePtr);
						}
						if(colorbuffer != NULL){
							delete[] colorbuffer;
						}
						if(data != NULL){
							delete[] data;
						}
						return;
					}
				}
			}
		}while(currentpixel < pixelcount);
		fclose(filePtr);
	}
	else			//unsupported tga type
		return;
}

bool CTexture::SaveTGA(const char *filename){
	static GLubyte tgaHeader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
	FILE *pFile;
	GLubyte header[6];
	GLubyte temp;
	int i;

//	if(!data)
//		return false;

	if(data)
		delete[] data;
	data = new GLubyte[width*height*channels];

	pFile=fopen(filename, "wb");
	if(!pFile){
		fclose(pFile);
		return false;
	}
	//set color info
	int bits = this->channels*8;
	int size = this->width * this->height * this->channels;

	// Save the width and height
 	header[0] = width % 256;
 	header[1] = width / 256;
 	header[2] = height % 256;
	header[3] = height / 256;
	header[4] = bits;
	header[5] = 0;

	//write headers
	fwrite(tgaHeader, sizeof(tgaHeader), 1, pFile);
	fwrite(header, sizeof(header), 1, pFile);
	glReadPixels(0,0,width,height,format,type,data);

	//switch image from RGB to BGR
	for(i=0; i < size; i+=channels){
		temp = data[i];
		data[i]=data[i+2];
		data[i+2]=temp;
	}

	fwrite(data,size,1,pFile);
	fclose(pFile);
	return true;
}

/*
bool CTexture::SaveTGA(const char *filename){
	static GLubyte tgaHeader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
	FILE *pFile;
	GLubyte header[6];
	GLubyte *buffer, temp;
	unsigned int i;

	pFile=fopen(filename, "w");
	if(!pFile){
		fclose(pFile);
		return false;
	}
	//set color info
	int bits = this->channels*8;
	int size = this->width * this->height * this->channels;

	// Save the width and height
 	header[0] = width % 256;
 	header[1] = width / 256;
 	header[2] = height % 256;
	header[3] = height / 256;
	header[4] = bits;
	header[5] = 0;

	//write headers
//	fwrite(tgaHeader, sizeof(tgaHeader), 1, pFile);
//	fwrite(header, sizeof(header), 1, pFile);
//	buffer = new GLubyte[size];
//	memcpy(buffer, this->data, size);

	//switch image from RGB to BGR
//	for(i=0; i < this->width * this->height * channels; i+=channels){
//		temp = buffer[i];
//		buffer[i]=buffer[i=2];
//		buffer[i+2]=temp;
//	}

//	fwrite(buffer,size,1,pFile);

	fprintf(pFile,"width=%d\n",width);
	fprintf(pFile,"height=%d\n",height);
	fprintf(pFile,"channels=%d\n",channels);
	fprintf(pFile,"bits=%d\n",bits);
	fprintf(pFile,"size=%d\n",size);
	fprintf(pFile,"actual size =%d\n",sizeof(data));


	fclose(pFile);
	return true;
}
*/