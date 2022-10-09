#include "StringToWstring.hpp"
std::wstring StringToWstring(const char* s) {
	std::string str(s);
	std::wstring widestr = std::wstring(str.begin(), str.end());
	//const wchar_t* widecstr = widestr.c_str();
	return widestr;
}

std::string WstringToString(const wchar_t* ws) {
	std::wstring wstr(ws);
	const std::string s(wstr.begin(), wstr.end());
	return s;
}