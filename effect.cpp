//effect.cpp

#include "effect.h"
#include "resource.h"

extern CglWindow *g_win;

CEffect::CEffect(char *name, char *vert, char *frag){
	memset(this,0,sizeof(*this));
	shader=new CShader(vert, frag);
	params=new Param[MAXPARAMS];
	this->name=name;


}

CEffect::~CEffect(){

	if(shader){
		delete shader;
		shader=0;
	}
	if(params){
		delete[] params;
		params=0;
	}

}

//activate effect program for rendering
void CEffect::Bind(){
	glUseProgramObjectARB(shader->programObj);
	glUniform1iARB(glGetUniformLocationARB(shader->programObj, "srcImage"), 0);
	for(int i=0; i<numParams; ++i){
		glUniform1fARB(glGetUniformLocationARB(shader->programObj, params[i].uname), params[i].value*params[i].scale+params[i].offset);

	}

}

//initialize interactive effect
void CEffect::Init(){
	if(numParams > 0 && !IsWindow(g_win->hDialog)){
		active=1;
		for(int i=0; i<numParams; ++i){
			params[i].value=params[i].dvalue;
		}
		ShowDialog();
	}
}

//add parameter
//args: GUI name, uniform name, ...
//default scale=1, offset=0
//shader value = gui value * scale + offset
void CEffect::AddParam(char *name, char* uname, float min, float max, float step, float value, float scale, float offset){
	if(numParams >= MAXPARAMS)
		return;
	++numParams;
	params[numParams-1].name=name;
	params[numParams-1].uname=uname;
	params[numParams-1].min=min;
	params[numParams-1].max=max;
	params[numParams-1].step=step;
	params[numParams-1].value=value;
	params[numParams-1].dvalue=value;
	params[numParams-1].scale=scale;
	params[numParams-1].offset=offset;
	
}


void CEffect::SetParam(int index, float value){
	if(index < 0 || index >= MAXPARAMS)
		return;
	if(value < params[index].min)
		return;
	if(value > params[index].max)
		return;
	params[index].value=value;
}



void CEffect::ShowDialog(){
	InitCommonControls();

	switch(numParams){
		case 1:
			g_win->hDialog=CreateDialog(g_win->hinstance, MAKEINTRESOURCE(IDD_DIALOG), g_win->hwnd, (DLGPROC)DlgProc);
			break;
		case 2:
			g_win->hDialog=CreateDialog(g_win->hinstance, MAKEINTRESOURCE(IDD_DIALOG2), g_win->hwnd, (DLGPROC)DlgProc);
			break;
		case 3:
			g_win->hDialog=CreateDialog(g_win->hinstance, MAKEINTRESOURCE(IDD_DIALOG3), g_win->hwnd, (DLGPROC)DlgProc);
			break;
	}
	for(int i=0; i<numParams; ++i){
		params[i].handle=CreateWindow(TRACKBAR_CLASS,
			NULL,
			WS_CHILD | WS_VISIBLE | TBS_HORZ,
			10, 36+i*54,			// position 
			240, 20,		// size
			g_win->hDialog,
			0,
			g_win->hinstance,
			0);

		SendMessage(params[i].handle, TBM_SETRANGE,
			(WPARAM) true,
			(LPARAM) MAKELONG(params[i].min, params[i].max));

		SendMessage(params[i].handle, TBM_SETPAGESIZE, 
			0, (LPARAM) long(params[i].step));

		SendMessage(params[i].handle, TBM_SETPOS,
			(WPARAM) true,
			(LPARAM) long(params[i].dvalue));

	}
}