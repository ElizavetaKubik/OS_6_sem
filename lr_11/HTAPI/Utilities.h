#pragma once
#include <string>

#define MAX_PATH_LENGTH 1024

#define HT_API __declspec(dllexport)


namespace Utilities
{
	HT_API std::string GetModulePath();

	HT_API std::wstring ConvertStringToWstring(std::string inValue);

}