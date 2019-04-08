//shader.cpp
#include "shader.h"



CShader::CShader(const char *vert, const char *frag){
	memset(this,0,sizeof(*this));
	vertString = ReadShaderFile(vert);
	if(!vertString) return;
	vertProgString = (char*)vertString;
	vertProg = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	glShaderSourceARB(vertProg, 1, &vertProgString, NULL);
	glCompileShaderARB(vertProg);
	glGetObjectParameterivARB(vertProg, GL_OBJECT_COMPILE_STATUS_ARB, &bVertCompiled);
	glGetObjectParameterivARB(vertProg, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logSize);
	if(logSize>0){
		log = new char[logSize];
	}
	if(bVertCompiled == false){
		glGetInfoLogARB(vertProg, logSize, NULL, log);
		MessageBox(NULL, log, "Vertex Program Compile Error", MB_OK|MB_ICONEXCLAMATION);
		delete[] log;
		delete[] vertString;
		vertString=0;
		vertProgString=0;
		PostQuitMessage(0);
		return;
	}
	delete[] log;
	delete[] vertString;
	vertString=0;
	vertProgString=0;

	fragString = ReadShaderFile(frag);
	if(!fragString) return;
	fragProgString = (char*)fragString;
	fragProg = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	glShaderSourceARB(fragProg, 1, &fragProgString, NULL);
	glCompileShaderARB(fragProg);
	glGetObjectParameterivARB(fragProg, GL_OBJECT_COMPILE_STATUS_ARB, &bFragCompiled);
	glGetObjectParameterivARB(fragProg, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logSize);
	if(logSize>0){
		log = new char[logSize];
	}
	if(bFragCompiled == false){
		glGetInfoLogARB(fragProg, logSize, NULL, log);
		MessageBox(NULL, log, "Fragment Program Compile Error", MB_OK|MB_ICONEXCLAMATION);
		delete[] log;
		delete[] fragString;
		fragString=0;
		fragProgString=0;
		PostQuitMessage(0);
		return;
	}
	delete[] log;
	delete[] fragString;
	fragString=0;
	fragProgString=0;

	programObj = glCreateProgramObjectARB();
    glAttachObjectARB(programObj, vertProg);
    glAttachObjectARB(programObj, fragProg);

	glLinkProgramARB(programObj);
	glGetObjectParameterivARB(programObj, GL_OBJECT_LINK_STATUS_ARB, &bLinked);
	glGetObjectParameterivARB(programObj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logSize);
	if(logSize>0){
		log = new char[logSize];
	}
	glGetInfoLogARB(programObj, logSize, NULL, log);
    if(bLinked == false){
		MessageBox(NULL, log, "Linking Error", MB_OK|MB_ICONEXCLAMATION);
		delete[] log;
		PostQuitMessage(0);
		return;
	}
	if(logSize>0 && strstr(log, "software")){
		supported=false;
	}
	else{
		supported=true;
	}
	delete[] log;
	log=0;
}

CShader::~CShader(){
//the 2 commands below cause a crash on exit
//	glDetachObjectARB(programObj, vertProg);
//	glDetachObjectARB(programObj, fragProg);
	
	if(vertProg){
		glDeleteObjectARB(vertProg);
		vertProg=0;
	}
	if(fragProg){
		glDeleteObjectARB(fragProg);
		fragProg=0;
	}
	if(programObj){
		glDeleteObjectARB(programObj);
		programObj=0;
	}


}

unsigned char* CShader::ReadShaderFile(const char *fileName){
	unsigned char *content=0;
	int count=0;
	FILE *file=fopen(fileName,"r");
    if(file==NULL){
        MessageBox( NULL, "Cannot open shader file!", "ERROR",
            MB_OK | MB_ICONEXCLAMATION );
		return 0;
    }
	fseek(file, 0, SEEK_END);
	count=ftell(file);
	rewind(file);
	if(count>0){
		content=(unsigned char*)malloc(sizeof(char)*(count+1));
		count = fread(content,sizeof(char),count,file);
		content[count] = '\0';
	}
	fclose(file);
	return content;
}

void CShader::Bind(){
	glUseProgramObjectARB(programObj);
}