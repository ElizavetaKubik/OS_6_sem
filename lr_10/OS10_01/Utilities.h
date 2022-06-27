#pragma once
#include <string>

#define MAX_PATH_LENGTH 1024

namespace Utilities
{
	std::string GetModulePath();

	std::wstring ConvertStringToWstring(std::string inValue);
}