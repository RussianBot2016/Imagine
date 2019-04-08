//glWindow.h

#ifndef __GLWINDOW_H
#define __GLWINDOW_H

#include "main.h"


class CglWindow{
public:
	HINSTANCE hinstance;
	HWND hwnd;
	HWND hDialog;
	HDC hdc;
	HGLRC hrc;
	int width, height, depth;	//screen width, height, color depth
	bool fullscreen;
	float frameinterval;
	char *name;					//window caption
	char *extensions;	//string of supported gl extensions
	GLuint FontListID;
	char *strfps;


	CglWindow(char* n, int w, int h, int d, bool fs);
	~CglWindow();
	int MakeWindow();
	
	void ChangeToFullScreen();


	void calcfps();
	bool CheckExtension(const char *search);
	unsigned int CreateBitmapFont(char *fontName, int fontSize);
	void glDrawText(int x, int y, char *str);
	void ClearFont();
	void SizeOpenGLScreen();
	void SizeRTScreen(int width, int height);
	void viewOrtho();
	void viewPerspective();

private:
	void SetupPixelFormat();
	void InitOpenGL();

};


#endif