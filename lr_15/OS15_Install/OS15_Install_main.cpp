#include <Windows.h>
#include <iostream>
#include <fstream>

#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>

#define SERVICENAME L"AA_WinService"
#define SERVICE_EXENAME "OS15_WinService.exe"


std::wstring ConvertStringToWstring(std::string inValue)
{
	std::wstring temp;
	wchar_t* temp_warray = new wchar_t[inValue.length() + 2];
	unsigned int wc_out_size;

	mbstowcs_s(&wc_out_size, temp_warray, inValue.length() + 2, inValue.c_str(), inValue.length());

	return temp_warray;
}

char* errortxt(const char* msg, int code)
{
	char* buf = new char[512];
	sprintf_s(buf, 512, "%s: error code = %d\n", msg, code);
	return buf;
}

int main()
{
	SC_HANDLE schService = NULL, schSCManager = NULL;
	try
	{
		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (!schSCManager) throw errortxt("OpenSCManager", GetLastError());
		else
		{
			char cwd[MAX_PATH];
			GetModuleFileNameA(NULL, cwd, MAX_PATH);
			std::string path(cwd);
			int pos = path.find_last_of("\\") + 1;
			std::wstring exePath = ConvertStringToWstring(path.substr(0, pos) + SERVICE_EXENAME);

			schService = CreateService(schSCManager, SERVICENAME, SERVICENAME, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, exePath.c_str(), NULL, NULL, NULL, NULL, NULL);
			if (!schService) throw errortxt("CreateService", GetLastError());
		}
	}
	catch (char* txt)
	{
		std::cout << txt << "\n";
	}
	if (schSCManager) CloseServiceHandle(schSCManager);
	if (schService) CloseServiceHandle(schSCManager);
	system("pause");
}