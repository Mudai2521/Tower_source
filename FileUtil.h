#pragma once
#include <string>
#include <Shlwapi.h>


#pragma comment( lib, "shlwapi.lib ")

bool SearchFilePath(const wchar_t* filename, std::wstring& result);
