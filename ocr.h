
#include <iostream>
#include <cstdio>
#include <fstream>
#include <windows.h>

using namespace std;

namespace _ocr {
	typedef void (* __stdcall VOID_FUNC)();
	typedef char* (* __stdcall OCR_FUNC)(const char*, int, int*);
	HMODULE hModule = LoadLibrary("c_ocr.dll");
	VOID_FUNC init = (VOID_FUNC)GetProcAddress(hModule, "init");
	OCR_FUNC ocr = (OCR_FUNC)GetProcAddress(hModule, "ocr");
	VOID_FUNC cleanup = (VOID_FUNC)GetProcAddress(hModule, "cleanup");
}

void initOCR() {
	_ocr::init();
}

string doOCR(const string &filename) {
	filebuf* pbuf;
	ifstream filestr;
	long size;
	char* buffer;
	filestr.open (filename, ios::binary);
	pbuf = filestr.rdbuf();
	size = pbuf->pubseekoff (0, ios::end, ios::in);
	pbuf->pubseekpos (0, ios::in);
	buffer = new char[size + 1];
	pbuf->sgetn (buffer, size);
	buffer[size] = '\0';
	filestr.close();
	int rsize = 0;
	char* ret = _ocr::ocr(buffer, size, &rsize);
	char txt[rsize + 1] = {};
	memcpy(txt, ret, rsize);
	free(ret);
	return txt;
}
