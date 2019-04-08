#ifndef __EFFECT_H
#define __EFFECT_H

#include "main.h"
#include "shader.h"
#include "glwindow.h"


#define MAXPARAMS 5

typedef struct{
	char *name;		//name in menu
	char *uname;	//uniform name in the shader
	float value;
	float dvalue;	//default value;
	float min;
	float max;
	float step;
	float scale;	//scale between true vale and shown value
	float offset;	//offset between true value and (shown value * scale)
	HWND handle;
} Param;

//extern WNDCLASSEX cwc;
//LRESULT CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

class CEffect{
public:
	CEffect(char *name, char *vert, char *frag);
	~CEffect();
	void Bind();
	void Init();
	void AddParam(char *name, char* uname, float min, float max, float step, float value, float scale=1.0f, float offset=0);
	void SetParam(int index, float value);
	void ShowDialog();
	CShader *shader;
	Param *params;
	int numParams;
	char *name;
	int active;

private:


};



#endif