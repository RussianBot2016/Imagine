//file.cpp
#include "main.h"

void ChoseFileOpen(OPENFILENAME &ofn, HWND hwnd, char *filename){
	char szFileName[MAX_PATH] = "";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Readable Files (*.bmp, *.tga)\0*.bmp;*.tga\0\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "bmp";

//	if(GetOpenFileName(&ofn)){
//		LoadTextFileToEdit(szFileName);
//	}
}

void ChoseFileSave(OPENFILENAME &ofn, HWND hwnd, char *filename){
	char szFileName[MAX_PATH] = "";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Targa Files (*.tga)\0*.tga\0\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "tga";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

//	if(GetSaveFileName(&ofn)){
//		SaveTextFileFromEdit(szFileName);
//	}
}