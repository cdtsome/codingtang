
#pragma once

#include <fstream>
#include <windows.h>
#include <string>
#include <direct.h>

using namespace std;

class Http {
	private:
		typedef const char* (CALLBACK __stdcall* HttpGetFunc)(const char*, const char*, const char*);
		typedef const char* (CALLBACK __stdcall* HttpPostFunc)(const char*, const char*, const char*, const char*);
		HttpGetFunc __Get;
		HttpPostFunc __Post;
		string RandString(size_t size) {
			string ret;
			for (size_t i = 0; i < size; i++) {
				ret += rand() % 26 + 'a';
			}
			return ret;
		}
	public:
		Http() {
			HMODULE hHttp = LoadLibraryA("http.dll");
			__Get = (HttpGetFunc)GetProcAddress(hHttp, "Get");
			__Post = (HttpPostFunc)GetProcAddress(hHttp, "Post");
			srand(time(0) + getpid());
			mkdir("temp");
		}
		string ToUtf8(string strAnsi) {
			UINT nLen = MultiByteToWideChar(936, 0, strAnsi.c_str(), -1, NULL, 0);
			WCHAR* wszBuffer = new WCHAR[nLen + 1];
			nLen = MultiByteToWideChar(936, 0, strAnsi.c_str(), -1, wszBuffer, nLen);
			wszBuffer[nLen] = 0;
			nLen = WideCharToMultiByte(CP_UTF8, 0, wszBuffer, -1, NULL, 0, NULL, NULL);
			CHAR* szBuffer = new CHAR[nLen + 1];
			nLen = WideCharToMultiByte(CP_UTF8, 0, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
			szBuffer[nLen] = 0;
			string s = szBuffer;
			delete []wszBuffer;
			delete []szBuffer;
			return s;
		}
		string ToAnsi(string szU8) {
			int wcsLen = MultiByteToWideChar(CP_UTF8, 0, szU8.c_str(), strlen(szU8.c_str()), 0, 0);
			wchar_t* wszMultiByte = new wchar_t[wcsLen + 1];
			MultiByteToWideChar(CP_UTF8, 0, szU8.c_str(), strlen(szU8.c_str()), wszMultiByte, wcsLen);
			wszMultiByte[wcsLen] = '\0';
			int ansiLen = WideCharToMultiByte(CP_ACP, 0, wszMultiByte, wcslen(wszMultiByte), 0, 0, 0, 0);
			char* szAnsi = new char[ansiLen + 1];
			szAnsi[ansiLen] = '\0';
			WideCharToMultiByte(CP_ACP, 0, wszMultiByte, wcslen(wszMultiByte), szAnsi, ansiLen, 0, 0);
			string s = szAnsi;
			delete []szAnsi;
			delete []wszMultiByte;
			return s;
		}
		string URLCoding(string text) {
			char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
			string s = ToUtf8(text), ret;
			for (int i = 0; i < s.size(); i++) {
				if (isalnum(s[i]) || s[i] == '.' || s[i] == '-' || s[i] == '*' || s[i] == '_') {
					ret += s[i];
				} else if (s[i] == ' ') {
					ret += "+";
				} else {
					unsigned char t = s[i];
					ret = ret + "%" + hex[t / 16] + hex[t % 16];
				}
			}
			return ret;
		}
		string Get(string Url, string Headers = "") {
			string fn = "temp\\" + RandString(16);
			__Get(Url.c_str(), Headers.c_str(), fn.c_str());
			ifstream ifs(fn.c_str());
			string ret((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
			ifs.close();
			remove(fn.c_str());
			return ToAnsi(ret);
		}
		string Post(string Url, string Text = "", string Headers = "") {
			string fn = "temp\\" + RandString(16);
			__Post(Url.c_str(), Text.c_str(), Headers.c_str(), fn.c_str());
			ifstream ifs(fn.c_str());
			string ret((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
			ifs.close();
			remove(fn.c_str());
			return ToAnsi(ret);
		}
		void Download(string Url, string SaveTo, string Headers = "") {
			__Get(Url.c_str(), Headers.c_str(), SaveTo.c_str());
		}
};
