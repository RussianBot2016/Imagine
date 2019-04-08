//main.h
#ifndef __MAIN_H
#define __MAIN_H


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"
#include "wglext.h"
#include "resource.h"




#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

//gloabal constants
#define CLASSNAME "OpenGL Win32 Class"
#define	CHILDCLASS "Dialog Control"
#define SHADERPATH "shaders\\"
#define MAXTEXTURES 10
#define MENUHEIGHT 20
#define MAXEFFECTS 20


typedef struct{
	float left;
	float right;
	float top;
	float bottom;
} fRect;

//global vars
extern PFNGLWINDOWPOS2IARBPROC glWindowPos2iARB;
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB;
extern PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC         glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC        glCompileShaderARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
extern PFNGLATTACHOBJECTARBPROC         glAttachObjectARB;
extern PFNGLGETINFOLOGARBPROC           glGetInfoLogARB;
extern PFNGLLINKPROGRAMARBPROC          glLinkProgramARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB;
extern PFNGLUNIFORM4FARBPROC            glUniform4fARB;
extern PFNGLUNIFORM3FARBPROC            glUniform3fARB;
extern PFNGLUNIFORM2FARBPROC            glUniform2fARB;
extern PFNGLUNIFORM1FARBPROC            glUniform1fARB;
extern PFNGLUNIFORM1IARBPROC            glUniform1iARB;
extern PFNGLUNIFORM1FVARBPROC           glUniform1fvARB;
extern PFNGLUNIFORM2FVARBPROC           glUniform2fvARB;
extern PFNGLUNIFORM3FVARBPROC           glUniform3fvARB;
extern PFNGLUNIFORM4FVARBPROC           glUniform4fvARB;
extern PFNGLDETACHOBJECTARBPROC			glDetachObjectARB;
extern PFNGLISRENDERBUFFEREXTPROC				glIsRenderbufferEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC				glBindRenderbufferEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC			glDeleteRenderbuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC				glGenRenderbuffersEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC			glRenderbufferStorageEXT;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC	glGetRenderbufferParameterivEXT;
extern PFNGLISFRAMEBUFFEREXTPROC				glIsFramebufferEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC				glBindFramebufferEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC			glDeleteFramebuffersEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC				glGenFramebuffersEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC		glCheckFramebufferStatusEXT;
extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC			glFramebufferTexture1DEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC			glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC			glFramebufferTexture3DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC		glFramebufferRenderbufferEXT;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
extern PFNGLGENERATEMIPMAPEXTPROC				glGenerateMipmapEXT;

extern LPDLGTEMPLATE pDlgTpl;



//function prototypes
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, LPSTR cmdline, int show);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK DlgProc(HWND hdlg, UINT message, WPARAM wparam, LPARAM lparam);
void Render();
void Init();
void CleanUp();
void DrawInfo();
void BuildMenu(HMENU &menu);
void AdjustQuad(fRect *quad, int texWidth, int texHeight);
void ResetView();
void ReadInput();
int InitShaders();
void DeleteTextures();
void DeleteFBO();
int InitFBO();
int InitEffects();

#endif
