//main.cpp
/*
gpu-based image processing

Adding Effects:
add ID to menu.h
add ID handler to WinProc
define effect in InitEffects()




*/
#include "main.h"
#include "menu.h"
#include "glWindow.h"
#include "shader.h"
#include "texture.h"
#include "file.h"
#include "effect.h"




#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "comctl32.lib")


#ifdef _DEBUG
	#include <crtdbg.h>
	#pragma comment(lib, "LIBCMTD.lib")  //use LIBCMTD for multi-threaded apps
	char debug[64];
	char debug2[64];
#endif




//globals
CglWindow* g_win;
CTexture *g_textures[MAXTEXTURES];
CEffect *g_effects[MAXEFFECTS];
HMENU g_menu;
int readTex = 0;
int writeTex = 1;
POINT mouse, oldmouse;
float zDelta = 0;
float g_scaleX=1.0f;
float g_scaleY=1.0f;
float g_scaleZ=1.0f;
float g_dx=0;
float g_dy=0;
float g_rx=0;
float g_ry=0;
float g_rz=0;
float g_rxp=0;
float g_ryp=0;
float g_rzp=0;
float g_rotate=0;
bool lbutton, rbutton;
fRect screenQuad = {0};
int numEffects;
int doEffect = -1;	//-1 if none, otherwise index into effects array
GLuint fbo;
const GLenum renderTarget[]={GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};



//prototypes
int SetFBO(CTexture *t0, CTexture *t1);

// GL function pointers
PFNGLWINDOWPOS2IARBPROC glWindowPos2iARB = 0;

// multitexturing extensions
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = 0;
PFNGLACTIVETEXTUREARBPROC   glActiveTextureARB = 0;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = 0;

//swap interval extension
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = 0;

//shader functions
PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB		=0;
PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB				=0;
PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB			=0;
PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB		=0;
PFNGLSHADERSOURCEARBPROC         glShaderSourceARB				=0;
PFNGLCOMPILESHADERARBPROC        glCompileShaderARB				=0;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB		=0;
PFNGLATTACHOBJECTARBPROC         glAttachObjectARB				=0;
PFNGLGETINFOLOGARBPROC           glGetInfoLogARB				=0;
PFNGLLINKPROGRAMARBPROC          glLinkProgramARB				=0;
PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB		=0;
PFNGLUNIFORM4FARBPROC            glUniform4fARB					=0;
PFNGLUNIFORM3FARBPROC            glUniform3fARB					=0;
PFNGLUNIFORM2FARBPROC            glUniform2fARB					=0;
PFNGLUNIFORM1FARBPROC            glUniform1fARB					=0;
PFNGLUNIFORM1IARBPROC            glUniform1iARB					=0;
PFNGLUNIFORM1FVARBPROC           glUniform1fvARB				=0;
PFNGLUNIFORM2FVARBPROC           glUniform2fvARB				=0;
PFNGLUNIFORM3FVARBPROC           glUniform3fvARB				=0;
PFNGLUNIFORM4FVARBPROC           glUniform4fvARB				=0;
PFNGLDETACHOBJECTARBPROC		 glDetachObjectARB				=0;

//FBO pointers
PFNGLISRENDERBUFFEREXTPROC				glIsRenderbufferEXT = 0;
PFNGLBINDRENDERBUFFEREXTPROC			glBindRenderbufferEXT = 0;
PFNGLDELETERENDERBUFFERSEXTPROC			glDeleteRenderbuffersEXT = 0;
PFNGLGENRENDERBUFFERSEXTPROC			glGenRenderbuffersEXT = 0;
PFNGLRENDERBUFFERSTORAGEEXTPROC			glRenderbufferStorageEXT = 0;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC	glGetRenderbufferParameterivEXT = 0;
PFNGLISFRAMEBUFFEREXTPROC				glIsFramebufferEXT = 0;
PFNGLBINDFRAMEBUFFEREXTPROC				glBindFramebufferEXT = 0;
PFNGLDELETEFRAMEBUFFERSEXTPROC			glDeleteFramebuffersEXT = 0;
PFNGLGENFRAMEBUFFERSEXTPROC				glGenFramebuffersEXT = 0;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC		glCheckFramebufferStatusEXT = 0;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC		glFramebufferTexture1DEXT = 0;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC		glFramebufferTexture2DEXT = 0;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC		glFramebufferTexture3DEXT = 0;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC		glFramebufferRenderbufferEXT = 0;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = 0;
PFNGLGENERATEMIPMAPEXTPROC				glGenerateMipmapEXT = 0;



int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, LPSTR cmdline, int show){
	#ifdef _DEBUG
		int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
		flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
		_CrtSetDbgFlag(flag); // Set flag to the new value
	#endif

	MSG msg;
	g_win=new CglWindow("Imagine",1024,768,32,true);	//will automatically resize to current resolution
	if(!g_win->hwnd){
		return 0;
	}

	Init();

	int done=0;
	while(!done){
		if(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)){
			if(msg.message==WM_QUIT){
				done=1;
			}
			if(!IsWindow(g_win->hDialog) || !IsDialogMessage(g_win->hDialog, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else{
			Render();
			ReadInput();
		}
	}
	CleanUp();
	return msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam){
	OPENFILENAME ofn = {0};
	char filename[MAX_PATH] = "";
	switch(message){
		case WM_KEYDOWN:
			/*
			switch(wparam){
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
				}
			*/
			return 0;

		case WM_COMMAND:
			switch(LOWORD(wparam)){
				case ID_OPEN:
					ChoseFileOpen(ofn,g_win->hwnd,filename);
					if(GetOpenFileName(&ofn)){
						DeleteTextures();
						readTex = 0;
						writeTex = 1;
						doEffect = -1;
						g_textures[readTex]=new CTexture(filename,GL_LINEAR);
						g_textures[writeTex]=new CTexture(g_textures[readTex]->width, g_textures[readTex]->height, g_textures[readTex]->channels, GL_LINEAR);
						AdjustQuad(&screenQuad, g_textures[readTex]->width, g_textures[readTex]->height);
						if(!SetFBO(g_textures[readTex], g_textures[writeTex])){
							MessageBox(NULL,"Can't setup FBO","Error",MB_OK);
						}
						ResetView();
					}
					break;

				case ID_SAVEAS:
					if(g_textures[readTex] == 0)
						break;
					ChoseFileSave(ofn,g_win->hwnd,filename);
					if(GetSaveFileName(&ofn)){
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
						glReadBuffer(renderTarget[readTex]);
						g_textures[readTex]->SaveTGA(filename);
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
					}					
					break;

				case ID_EXIT:
					PostQuitMessage(0);
					break;

				case ID_UNDO:
					readTex ^= 1;
					writeTex ^= 1;
					break;

				case ID_FILTER1:
					doEffect=0;
					break;

				case ID_FILTER2:
					doEffect=1;
					break;

				case ID_FILTER3:
					doEffect=2;
					break;

				case ID_FILTER4:
					doEffect=3;
					break;

				case ID_FILTER5:
					doEffect=4;
					break;

				case ID_FILTER6:
					doEffect=5;
					break;

				case ID_FILTER7:
					doEffect=6;
					break;

				case ID_FILTER8:
					doEffect=7;
					break;

				case ID_FILTER9:
					doEffect=8;
					break;

				case ID_FILTER10:
					doEffect=9;
					break;

				case ID_FILTER11:
					doEffect=10;
					break;

				case ID_FILTER12:
					doEffect=11;
					break;
			}
			if(doEffect>=0 && g_textures[readTex] != 0){
				g_effects[doEffect]->Init();
			}
			return 0;

		case WM_SYSCOMMAND:
			switch (wparam){
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;

		case WM_LBUTTONDOWN:
			lbutton=true;
			GetCursorPos(&oldmouse);

			break;

		case WM_LBUTTONUP:
			lbutton=false;
			break;

		case WM_MBUTTONDOWN:
			ResetView();
			break;

		case WM_RBUTTONDOWN:
			rbutton=true;
			GetCursorPos(&oldmouse);
			break;

		case WM_RBUTTONUP:
			rbutton=false;
			break;


		case WM_MOUSEWHEEL:
			zDelta = (short)HIWORD(wparam);
			if(zDelta < 0){
				g_scaleZ +=g_scaleZ*0.05f;
			}
			if(zDelta > 0){
				g_scaleZ -=g_scaleZ*0.05f;
			}
			break;

		case WM_CLOSE:
		case WM_QUIT:
			PostQuitMessage(0);
			return 0;
	}
	return(DefWindowProc(hwnd,message,wparam,lparam));
}



void Render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();


	if(g_textures[readTex] != 0){
		g_textures[readTex]->Bind();
		if(doEffect >= 0){
			g_effects[doEffect]->Bind();
			
			if(!g_effects[doEffect]->active){
				//process effect
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
				g_win->SizeRTScreen(g_textures[readTex]->width, g_textures[readTex]->height);
				glDrawBuffer(renderTarget[writeTex]);

				glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex2i(0, 0);
				glTexCoord2i(g_textures[readTex]->width, 0);
				glVertex2i(g_textures[readTex]->width, 0);
				glTexCoord2i(g_textures[readTex]->width, g_textures[readTex]->height);
				glVertex2i(g_textures[readTex]->width, g_textures[readTex]->height);
				glTexCoord2i(0, g_textures[readTex]->height);
				glVertex2i(0, g_textures[readTex]->height);	
				glEnd();
				glUseProgramObjectARB(0);

				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				readTex ^= 1;
				writeTex ^= 1;
				doEffect=-1;

				//render effect immediately after processing
				g_win->SizeOpenGLScreen();
				glScalef(g_scaleZ, g_scaleZ, g_scaleZ);
				glTranslatef(g_dx,g_dy,0);
				glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex2f(screenQuad.left, screenQuad.bottom);
				glTexCoord2i(g_textures[readTex]->width, 0);
				glVertex2f(screenQuad.right, screenQuad.bottom);
				glTexCoord2i(g_textures[readTex]->width, g_textures[readTex]->height);
				glVertex2f(screenQuad.right, screenQuad.top);
				glTexCoord2i(0, g_textures[readTex]->height);
				glVertex2f(screenQuad.left, screenQuad.top);
				glEnd();

			}
			else{
				//show interactive effect but dont process
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				g_win->SizeOpenGLScreen();

				glScalef(g_scaleZ, g_scaleZ, g_scaleZ);
				glTranslatef(g_dx,g_dy,0);
				
				if(doEffect==6){
					g_win->viewPerspective();
					g_rz=g_effects[doEffect]->params[2].value;
					g_ry=g_effects[doEffect]->params[1].value;
					g_rx=g_effects[doEffect]->params[0].value;
					glRotatef(g_rz, 0,0,1);
					glRotatef(g_ry, 0,1,0);
					glRotatef(g_rx, 1,0,0);
				}
				

				
				glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex2f(screenQuad.left, screenQuad.bottom);
				//glVertex2i(-int(g_textures[readTex]->width)/2, -int(g_textures[readTex]->height)/2);
				glTexCoord2i(g_textures[readTex]->width, 0);
				glVertex2f(screenQuad.right, screenQuad.bottom);
				//glVertex2i((g_textures[readTex]->width)/2, -int(g_textures[readTex]->height)/2);
				glTexCoord2i(g_textures[readTex]->width, g_textures[readTex]->height);
				glVertex2f(screenQuad.right, screenQuad.top);
				//glVertex2i((g_textures[readTex]->width)/2, (g_textures[readTex]->height)/2);
				glTexCoord2i(0, g_textures[readTex]->height);
				glVertex2f(screenQuad.left, screenQuad.top);
				//glVertex2i(-int(g_textures[readTex]->width)/2, (g_textures[readTex]->height)/2);
				glEnd();
				glUseProgramObjectARB(0);
				
				if(doEffect==6){
					g_win->viewOrtho();
				}	
				
			}
		}

		else{
			//no effect
			g_win->SizeOpenGLScreen();
			glScalef(g_scaleZ, g_scaleZ, g_scaleZ);
			glTranslatef(g_dx,g_dy,0);
			glRotatef(g_rz, 0,0,1);
			glRotatef(g_ry, 0,1,0);
			glRotatef(g_rx, 1,0,0);
			

			glBegin(GL_QUADS);
			glTexCoord2i(0, 0);
			glVertex2f(screenQuad.left, screenQuad.bottom);
			glTexCoord2i(g_textures[readTex]->width, 0);
			glVertex2f(screenQuad.right, screenQuad.bottom);
			glTexCoord2i(g_textures[readTex]->width, g_textures[readTex]->height);
			glVertex2f(screenQuad.right, screenQuad.top);
			glTexCoord2i(0, g_textures[readTex]->height);
			glVertex2f(screenQuad.left, screenQuad.top);
			glEnd();
		}

	}

	glPopMatrix();


/*	//initial screen
	if(g_textures[readTex] == 0){
		glPushAttrib(GL_CURRENT_BIT);
		glBegin(GL_QUADS);
		glColor3f(0.0f,0.0f,0.0f);
		//glTexCoord2f(0, 1);
		glVertex2f(screenQuad.left, screenQuad.bottom);

		glColor3f(0.5f,0.5f,0.5f);
		//glTexCoord2f(0, 0);
		glVertex2f(screenQuad.right, screenQuad.bottom);

		glColor3f(0.5f,0.5f,0.5f);
		//glTexCoord2f(1, 0);
		glVertex2f(screenQuad.right, screenQuad.top);

		glColor3f(0.0f,0.0f,0.0f);
		//glTexCoord2f(1, 1);
		glVertex2f(screenQuad.left, screenQuad.top);
		glEnd();
		glPopAttrib();
	}
*/

#ifdef _DEBUG
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	float maxbuffers;
	glGetFloatv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxbuffers);
	if(readTex)
		sprintf(debug,"texture: %d x %d",(int)g_textures[readTex]->width, (int)g_textures[readTex]->height);
	g_win->glDrawText(0,20,debug);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
#endif

	glFinish();
	SwapBuffers(g_win->hdc);
}



void Init(){
	if(!InitShaders()){
		MessageBox(NULL, "Shaders not supported", "Error", MB_OK);
		PostQuitMessage(0);
	}
	if(!InitFBO()){
		MessageBox(NULL, "Can't initialize FBO", "Error", MB_OK);
		PostQuitMessage(0);
	}
	g_win->FontListID=g_win->CreateBitmapFont("Arial",16);
	numEffects=InitEffects();
	BuildMenu(g_menu);
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if(wglSwapIntervalEXT){
		wglSwapIntervalEXT(1);
	}
	if(g_win->CheckExtension("GL_ARB_window_pos"))
		glWindowPos2iARB = (PFNGLWINDOWPOS2IARBPROC)wglGetProcAddress("glWindowPos2iARB");

	AdjustQuad(&screenQuad,g_win->width,g_win->height);
	//SetBkColor(g_win->hdc, RGB(0,0,0));

}



void CleanUp(){
	int i;
	DestroyMenu(g_menu);
	g_win->ClearFont();
	for(i=0; i<numEffects; ++i){
		if(g_effects[i]){
			delete g_effects[i];
			g_effects[i]=0;
		}
	}

	DeleteFBO();
	DeleteTextures();
	g_win->ClearFont();
	if(g_win)
		delete g_win;
}



void DeleteTextures(){
	for(int i=0; i<MAXTEXTURES; ++i){
		if(g_textures[i]){
			delete g_textures[i];
			g_textures[i]=0;
		}
	}
}



void DeleteFBO(){
	if(fbo){
		glDeleteFramebuffersEXT(1, &fbo);
		fbo=0;
	}
}



LRESULT CALLBACK DlgProc(HWND hdlg, UINT message, WPARAM wparam, LPARAM lparam){
	LRESULT dwPos;
	unsigned int name;
	unsigned int value;
	int i;
	switch(message){
		case WM_INITDIALOG:
			name=IDC_NAME;
			value=IDC_VALUE;
			for(i=0; i<g_effects[doEffect]->numParams; ++i){
				dwPos=SendMessage(g_effects[doEffect]->params[i].handle, TBM_GETPOS, 0, 0);
				SetDlgItemText(hdlg,name,g_effects[doEffect]->params[i].name);
				SetDlgItemInt(hdlg,value,(int)g_effects[doEffect]->params[i].dvalue,true);
				++name;
				++value;
			}
			SetWindowText(hdlg,g_effects[doEffect]->name);
			break;
		case WM_KEYDOWN:
			switch(wparam){
				case VK_ESCAPE:
					DestroyWindow(hdlg);
					break;
				}
			break;
		case WM_COMMAND:
			switch(LOWORD(wparam)){
				case IDD_CANCEL:
					DestroyWindow(hdlg);
					if(g_effects[doEffect])
						g_effects[doEffect]->active=0;
					if(doEffect == 6){
						g_rx=g_rxp;
						g_ry=g_ryp;
						g_rz=g_rzp;
					}
					doEffect=-1;
					break;
				case IDD_OK:
					DestroyWindow(hdlg);
					if(g_effects[doEffect])
						g_effects[doEffect]->active=0;
					if(doEffect == 6){
						g_rxp=g_rx;
						g_ryp=g_ry;
						g_rzp=g_rz;
					}
					break;
			}
			break;


		case WM_HSCROLL:
			switch(LOWORD(wparam)){
				case TB_ENDTRACK:
					value=IDC_VALUE;
					for(i=0; i<g_effects[doEffect]->numParams; ++i){
						dwPos=SendMessage(g_effects[doEffect]->params[i].handle, TBM_GETPOS, 0, 0);
						SetDlgItemInt(hdlg,value,dwPos,true);
						g_effects[doEffect]->SetParam(i,(float)dwPos);
						++value;
					}
					break;
			}
			return 0;
			break;


		case WM_CLOSE:
			DestroyWindow(hdlg);
			if(g_effects[doEffect])
				g_effects[doEffect]->active=0;
			doEffect=-1;
			break;
		default:
			return false;
	}
	return true;
}



void BuildMenu(HMENU &menu){
	menu = CreateMenu();
	HMENU popup = CreateMenu();
	AppendMenu(popup, MF_STRING, ID_OPEN, "&Open");
	AppendMenu(popup, MF_STRING, ID_SAVEAS, "&Save As");
	AppendMenu(popup, MF_SEPARATOR, 0, 0);
	AppendMenu(popup, MF_STRING, ID_EXIT, "E&xit");

	AppendMenu(menu, MF_POPUP, (UINT)popup, "&File");
	
	popup = CreateMenu();
	AppendMenu(popup, MF_STRING, ID_UNDO, "&Undo");
	AppendMenu(menu, MF_POPUP, (UINT)popup, "&Edit");
	
	popup = CreateMenu();
	unsigned int cmd = ID_FILTER1;

	for(int i=0; i<numEffects; ++i, ++cmd){
		if(g_effects[i]->shader->programObj){
			AppendMenu(popup, MF_STRING, cmd, g_effects[i]->name);
			if(i==6)
				AppendMenu(popup, MF_SEPARATOR, 0, 0);
		}
	}
	AppendMenu(menu, MF_POPUP, (UINT)popup, "&Filters");

	SetMenu(g_win->hwnd, menu);
}



void AdjustQuad(fRect *quad, int width, int height){
	if(width == 0 || height == 0){
		return;
	}
	quad->left=-(float(width/2));
	quad->top=float(height/2);
	quad->right=float(width/2);
	quad->bottom=-(float(height/2));
}



void ResetView(){
	g_scaleX=1.0f;
	g_scaleY=1.0f;
	g_scaleZ=1.0f;
	g_dx=0;
	g_dy=0;
}



void ReadInput(){
	if(lbutton){
		GetCursorPos(&mouse);
		if(oldmouse.x-mouse.x != 0 || mouse.y-oldmouse.y != 0){
			g_dx+=float(mouse.x-oldmouse.x)/g_scaleZ;
			g_dy+=float(oldmouse.y-mouse.y)/g_scaleZ;
			oldmouse.x=mouse.x;
			oldmouse.y=mouse.y;
		}		
	}
}



int InitShaders(){
    if(! g_win->CheckExtension("GL_ARB_shading_language_100")){
        return 0;
    }
    if(! g_win->CheckExtension("GL_ARB_shader_objects")){
        return 0;
    }
	if(! g_win->CheckExtension("GL_ARB_fragment_program")){
		return 0;
	}
    glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
    glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
    glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
    glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
    glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
    glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
    glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
    glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
    glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
    glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
    glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
    glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
	glUniform3fARB            = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
	glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
	glUniform2fARB            = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
	glUniform1fARB            = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
	glUniform1fvARB           = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress("glUniform1fvARB");

    if( !glCreateProgramObjectARB || !glDeleteObjectARB || !glUseProgramObjectARB ||
        !glCreateShaderObjectARB || !glCreateShaderObjectARB || !glCompileShaderARB || 
        !glGetObjectParameterivARB || !glAttachObjectARB || !glGetInfoLogARB || 
        !glLinkProgramARB || !glGetUniformLocationARB || !glUniform4fARB ||
		!glUniform1iARB || !glUniform2fARB || !glUniform1fARB || !glUniform3fARB || !glUniform1fvARB)
    {
        return 0;
    }
	return 1;
}




int InitFBO(){
	if(!g_win->CheckExtension("GL_EXT_framebuffer_object")){
		return 0;
	}
	glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
	glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
	glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

	if( !glIsRenderbufferEXT || !glBindRenderbufferEXT || !glDeleteRenderbuffersEXT || 
		!glGenRenderbuffersEXT || !glRenderbufferStorageEXT || !glGetRenderbufferParameterivEXT || 
		!glIsFramebufferEXT || !glBindFramebufferEXT || !glDeleteFramebuffersEXT || 
		!glGenFramebuffersEXT || !glCheckFramebufferStatusEXT || !glFramebufferTexture1DEXT || 
		!glFramebufferTexture2DEXT || !glFramebufferTexture3DEXT || !glFramebufferRenderbufferEXT||  
		!glGetFramebufferAttachmentParameterivEXT || !glGenerateMipmapEXT )
	{
		return 0;
	}
	return 1;
}



int SetFBO(CTexture *t0, CTexture *t1){

	if(fbo){
		glDeleteFramebuffersEXT(1, &fbo);
		fbo=0;
	}
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	
	//bind texture 0
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
                          t0->target, t0->texID, 0);
	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT){
		return 0;
	}
	
	//bind texture 1
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, 
                          t1->target, t1->texID, 0);
	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT){
		return 0;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	return 1;
}



int InitEffects(){
	int count=0;
	g_effects[count]=new CEffect("Invert", "shaders\\nullVS.txt","shaders\\invertFS.txt");
	++count;

	g_effects[count]=new CEffect("Luminance", "shaders\\nullVS.txt","shaders\\luminFS.txt");
	++count;

	g_effects[count]=new CEffect("Brightness", "shaders\\nullVS.txt","shaders\\brightFS.txt");
	g_effects[count]->AddParam("Amount:", "brightness", -100.0f, 100.0f, 1.0f, 0, 0.01f);
	++count;

	g_effects[count]=new CEffect("Contrast", "shaders\\nullVS.txt","shaders\\contrFS.txt");
	g_effects[count]->AddParam("Amount:", "contrast", -100.0f, 100.0f, 1.0f, 0, 0.01f, 1.0f);
	++count;

	g_effects[count]=new CEffect("Color Balance", "shaders\\nullVS.txt","shaders\\rgbFS.txt");
	g_effects[count]->AddParam("Red:", "red", -100.0f, 100.0f, 1.0f, 0, 0.01f);
	g_effects[count]->AddParam("Green:", "green", -100.0f, 100.0f, 1.0f, 0, 0.01f);
	g_effects[count]->AddParam("Blue:", "blue", -100.0f, 100.0f, 1.0f, 0, 0.01f);
	++count;

	g_effects[count]=new CEffect("Exposure", "shaders\\nullVS.txt","shaders\\expFS.txt");
	g_effects[count]->AddParam("Amount:", "exposure", -100.0f, 100.0f, 1.0f, 0, 0.01f, 1.0f);
	++count;

	g_effects[count]=new CEffect("Rotate", "shaders\\nullVS.txt","shaders\\nullFS.txt");
	g_effects[count]->AddParam("X:", "", -90.0f, 90.0f, 1.0f, g_rx);
	g_effects[count]->AddParam("Y:", "", -90.0f, 90.0f, 1.0f, g_ry);
	g_effects[count]->AddParam("Z:", "", -90.0f, 90.0f, 1.0f, g_rz);
	++count;

	g_effects[count]=new CEffect("Gaussian Blur", "shaders\\nullVS.txt","shaders\\gaussFS.txt");
	++count;

	g_effects[count]=new CEffect("Mean", "shaders\\nullVS.txt","shaders\\meanFS.txt");
	++count;

	g_effects[count]=new CEffect("Edge Detect", "shaders\\nullVS.txt","shaders\\edgeFS.txt");
	++count;

	g_effects[count]=new CEffect("Emboss", "shaders\\nullVS.txt","shaders\\embossFS.txt");
	++count;

	g_effects[count]=new CEffect("Sharpen", "shaders\\nullVS.txt","shaders\\sharpFS.txt");
	g_effects[count]->AddParam("Amount:", "scale", 0.0f, 100.0f, 1.0f, 10.0f, 0.01f);
	++count;

	return count;
}