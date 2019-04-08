//shader.h
#ifndef __SHADER_H
#define __SHADER_H

#include "main.h"



class CShader{
public:
	CShader(const char *vert, const char *frag);
	~CShader();
	void Bind();
	GLhandleARB programObj;
	bool supported;

private:
	unsigned char *ReadShaderFile(const char *fileName);
	unsigned char *vertString, *fragString;
	char *log;
	const char *vertProgString, *fragProgString;
    GLint bVertCompiled;
    GLint bFragCompiled;
    GLint bLinked;
	GLint logSize;
	GLhandleARB vertProg, fragProg;
};

#endif