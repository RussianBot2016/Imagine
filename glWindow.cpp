//glWindow.cpp

#include "glWindow.h"


//constructor
//args: name, width, height, color depth, fullscreen
CglWindow::CglWindow(char *n, int w, int h, int d, bool fs){
	memset(this,0,sizeof(*this));
	name=n;
	width=w;
	height=h;
	depth=d;
	fullscreen=fs;
	strfps=new char[32];
	MakeWindow();
	InitOpenGL();


}

//destructor
CglWindow::~CglWindow(){
	delete[] strfps;
	if(hrc){
		wglMakeCurrent(hdc, NULL);
		wglDeleteContext(hrc);
	}
	if(hdc){
		ReleaseDC(hwnd,hdc);
	}
	if(fullscreen){
		ChangeDisplaySettings(NULL,0);
//		ShowCursor(true);
	}
	DestroyWindow(hDialog);
	DestroyWindow(hwnd);
	UnregisterClass(CLASSNAME, hinstance);
}

//makes a Win32 window
int CglWindow::MakeWindow(){
	WNDCLASS wc;
	RECT rect;
	DWORD dwStyle;
	hinstance=GetModuleHandle(NULL);	
	//fill out window class struct
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style=CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc=WndProc;
	wc.hInstance=this->hinstance;
	wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName=CLASSNAME;
	wc.hCursor=LoadCursor(NULL, IDC_ARROW);
	//register class
	if(!RegisterClass(&wc)){
		MessageBox(NULL,"Cant register class","Error",MB_OK);
		return 0;
	}
	if (fullscreen){		
		dwStyle=WS_POPUP;
		ChangeToFullScreen();
	}
	else{
		dwStyle =WS_OVERLAPPEDWINDOW /*| WS_CAPTION | WS_SYSMENU*/;
	}
	rect.left=(long)0;
	rect.right=(long)width;
	rect.top=(long)0;
	rect.bottom=(long)height;
	AdjustWindowRect(&rect, dwStyle, false);
	//create window
	hwnd=CreateWindow(CLASSNAME,name,
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,0,
		rect.right-rect.left,
		rect.bottom-rect.top,
		NULL,
		NULL,
		hinstance,
		NULL);
	if(!hwnd){
		MessageBox(NULL,"Can't create main window","Error",MB_OK);
		return 0;
	}
	ShowWindow(hwnd,SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	UpdateWindow(hwnd);
	return 1;

}

void CglWindow::SetupPixelFormat(){
	int nPixelFormat;
	static PIXELFORMATDESCRIPTOR pfd={
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			depth,					//color mode
			0,0,0,0,0,0,			//color bits
			0,						//alpha buffer
			0,						//shift bit
			0,						//accumulation buffer
			0,0,0,0,				//accumulation bits
			24,						//depth buffer (z-buffer) bits
			0,						//stencil buffer bits
			0,						//aux buffer
			PFD_MAIN_PLANE,
			0,
			0,0,0
	};
	//choose best matching pixel format
	nPixelFormat=ChoosePixelFormat(hdc, &pfd);
	//set pixel format to device context
	SetPixelFormat(hdc,nPixelFormat,&pfd);
}


void CglWindow::ChangeToFullScreen(){
	DEVMODE dmScreenSettings, current;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &current);
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
	dmScreenSettings.dmSize = sizeof(dmScreenSettings);	
	dmScreenSettings.dmPelsWidth = current.dmPelsWidth;
	dmScreenSettings.dmPelsHeight = current.dmPelsHeight;
	dmScreenSettings.dmBitsPerPel = current.dmBitsPerPel;
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		// setting display mode failed, switch to windowed
		MessageBox(NULL, "Display mode failed", NULL, MB_OK);
		fullscreen=false;	
	}
	else{
		fullscreen=true;
		this->width=current.dmPelsWidth;
		this->height=current.dmPelsHeight;
		this->depth=current.dmBitsPerPel;
	}

}


void CglWindow::InitOpenGL(){
	hdc = GetDC(hwnd);
	SetupPixelFormat();
	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);
	extensions=(char*)glGetString(GL_EXTENSIONS);
	char *renderer = (char *)glGetString(GL_RENDERER);
	char *version = (char *)glGetString(GL_VERSION);
	if(strstr(renderer, "GDI Generic")){
		return;
	}
	if(strncmp(version, "1.1", 3)==0){
		return;
	}
	glClearColor(0.4f,0.4f,0.4f,0.0f);
	//setup other stuff
	glShadeModel(GL_SMOOTH);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	SizeOpenGLScreen();
}


void CglWindow::SizeOpenGLScreen(){
	if(this->height==0)
		this->height=1;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//left, right, bottom, top, near, far

	glOrtho(-this->width/2, this->width/2, -(this->height)/2+MENUHEIGHT,  (this->height)/2, -5000.0f, 5000.0f);
	//gluPerspective(60.0f, (GLfloat)this->width/(GLfloat)(this->height-MENUHEIGHT), 1.0f, 5000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0,0,this->width,this->height-MENUHEIGHT);
	gluLookAt(0,(GLfloat)MENUHEIGHT/2,(GLfloat)this->width/1.88f, 0,(GLfloat)MENUHEIGHT/2,0, 0,1.0f,0);
}

void CglWindow::SizeRTScreen(int width, int height){
	if(width<=0)
		width=1;
	if(height<=0)
		height=1;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, width, height);
}

//get framerate
void CglWindow::calcfps(){
	static float fps=0.0f;
	static float lasttime=0.0f;
	static float frametime=0.0f;
	float currtime=GetTickCount()*0.001f;
	frameinterval=currtime-frametime;
	frametime=currtime;
	fps++;
	if(currtime-lasttime > 1.0f){
		lasttime=currtime;
		sprintf(strfps,"fps: %d",int(fps));
		fps=0;
	}
}



//check if specifed extension is supported
bool CglWindow::CheckExtension(const char *search){
	unsigned int idx=0;
	char *str=extensions;
	if (str==NULL)
		return false;
	char *end=str+strlen(str);
	// loop while we haven't reached the end of the string
	while (str<end){
		// find where a space is located
		idx = strcspn(str," ");
		//found extension
		if((strlen(search)==idx) && (strncmp(search,str,idx)==0)){
			return true;
		}
		//didn't find extension, move pointer to the next string to search
		str+=(idx + 1);
	}
	return false;
}



unsigned int CglWindow::CreateBitmapFont(char *fontName, int fontSize){
	HFONT hFont;				// windows font
	unsigned int base;
	base = glGenLists(96);      // create storage for 96 characters

	if (stricmp(fontName, "symbol") == 0){
	     hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, SYMBOL_CHARSET, 
							OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							FF_DONTCARE | DEFAULT_PITCH, fontName);
	}
	else{
		 hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
							OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							FF_DONTCARE | DEFAULT_PITCH, fontName);
	}
	if (!hFont)
		return 0;
	SelectObject(hdc, hFont);
	wglUseFontBitmaps(hdc, 32, 96, base);
	return base;
}


void CglWindow::glDrawText(int x, int y, char *str){
	if ((FontListID == 0) || (FontListID == NULL))
		return;
	glPushAttrib(GL_LIST_BIT);
	glListBase(FontListID - 32);
	if(glWindowPos2iARB){
		glWindowPos2iARB(x,height-y-10);	//make y=0 the top
	}
	else{
		glRasterPos2i(x-width*0.5f,height*0.5f-y+10);
	}
	glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);
	glPopAttrib();
}


void CglWindow::ClearFont(){
	if (FontListID != 0)
		glDeleteLists(FontListID, 96);
}

//set ortho view
void CglWindow::viewOrtho(){
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//glPushMatrix();
	//glLoadIdentity();

	glOrtho(-this->width/2, this->width/2, -(this->height)/2+MENUHEIGHT,  (this->height)/2, -5000.0f, 5000.0f);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	//glPushMatrix();
	//glLoadIdentity();
}

//set perspective view
void CglWindow::viewPerspective(){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//glPopMatrix();

	gluPerspective(60.0f, (GLfloat)this->width/(GLfloat)(this->height-MENUHEIGHT), 1.0f, 5000.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0,(GLfloat)MENUHEIGHT/2,(GLfloat)this->width/1.88f, 0,(GLfloat)MENUHEIGHT/2,0, 0,1.0f,0);

	//glPopMatrix();


}