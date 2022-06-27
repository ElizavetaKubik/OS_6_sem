#pragma once
#include <Windows.h>
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#include <iostream>

#include "../OS15_HTAPI/Utilities.h"

#pragma comment(lib,"../Debug/OS15_HTAPI.lib")

#include <fstream>
#include "HT_Service.h"

#include "../OS15_HTAPI/HashTable.h"
#include "../OS15_HTAPI/HashTableAPI.h"
#include <string>
#include <algorithm>


#pragma comment(lib,"../Debug/OS15_HTAPI.lib")
using namespace std;

WCHAR ServiceName[] = SERVICENAME;
SERVICE_STATUS_HANDLE hServiceStatusHandle;
SERVICE_STATUS ServiceStatus;


void killProcessByName(const wchar_t* filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);

	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (wcscmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}


void trace(const char* msg, int r)
{
	char cwd[MAX_PATH];
	GetModuleFileNameA(NULL, cwd, MAX_PATH);
	string path(cwd);
	int pos = path.find_last_of("\\") + 1;
	string filePath = path.substr(0, pos) + "logs\\" + TRACEFILENAME;

	std::ofstream out;
	out.open(filePath, r);
	out << msg << "\n";
	out.close();
}
void trace(const wchar_t* msg, int r)
{
	char cwd[MAX_PATH];
	GetModuleFileNameA(NULL, cwd, MAX_PATH);
	string path(cwd);
	int pos = path.find_last_of("\\") + 1;
	string filePath = path.substr(0, pos) + "logs\\" + TRACEFILENAME;

	std::wofstream out;
	out.open(filePath, r);
	out << msg << "\n";
	out.close();
}

HT::HashTableData* ht = NULL;

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	trace("start: "); trace(SERVICENAME);

	char temp[121];
	wchar_t wtemp[121];

	ServiceStatus.dwServiceType = SERVICE_WIN32;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;

	if (!(hServiceStatusHandle = RegisterServiceCtrlHandler(ServiceName, ServiceHandler)))
	{
		sprintf_s(temp, "\nSetServiceStatusFailed, error code = %d\n", GetLastError());
		trace(temp);
	}
	else
	{

		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		if (!SetServiceStatus(hServiceStatusHandle, &ServiceStatus))
		{
			sprintf_s(temp, "\nSetSetviceStatus failed, error code =%d\n", GetLastError());
			trace(temp);
		}
		else
		{
			trace("start service", std::ofstream::out);
			try
			{
				HTAPI::OpenApi();

				ht = HTAPI::CreateFromFile(Utilities::GetModulePath() + "1.txt");

				sprintf_s(temp, "\nht:%d", (int)ht);
				trace(temp);
				if (ht != NULL)
				{
					sprintf_s(temp, "\nHT-Storage Created snapshotinterval=%d, capacity = %d, maxkeylength = %d, maxdatalength = %d",
						ht->SnapShotIntervalInSec, ht->Capacity, ht->MaxKeyLength, ht->MaxValueLength);
					trace(temp);
					swprintf_s(wtemp, L"user group:%s\n", ht->HTUsersGroup);
					trace(wtemp);
					swprintf_s(wtemp, L"\nFilemap name: %s\n", ht->FileMapName);
					trace(wtemp);
				}
				else
				{
					trace("\n error");
				}

				while (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
				{
					Sleep(3000);
					trace("\nservice with HT is in running state");
				}

				if (ht != NULL)
				{
					sprintf_s(temp, "\nht:%d", ht);
					trace(temp);
					sprintf_s(temp, "\nHT-Storage has been closed: snapshotinterval=%d, capacity = %d, maxkeylength = %d, maxdatalength = %d",
						ht->SnapShotIntervalInSec, ht->Capacity, ht->MaxKeyLength, ht->MaxValueLength);
					HTAPI::Close(ht);
				}
				else
				{
					trace("ht=null\n");
				}
			}
			catch (char* e)
			{
				trace(e);
			}

		}
	}

}


VOID WINAPI ServiceHandler(DWORD fdwControl)
{
	char temp[121];
	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:
	{
		killProcessByName(L"OS15_Start.exe");
		killProcessByName(L"OS15_Insert.exe");
		killProcessByName(L"OS15_Delete.exe");
		killProcessByName(L"OS15_Update.exe");
		trace("Service stoped\n");
	}

	case SERVICE_CONTROL_SHUTDOWN:
	{
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		trace("Service shutdowned\n");

		break;
	}

	case SERVICE_CONTROL_PAUSE:
		ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_INTERROGATE: break;
	default:
		sprintf_s(temp, "Unrecognized opcode %d\n", fdwControl);
		trace(temp);
	}

	if (!SetServiceStatus(hServiceStatusHandle, &ServiceStatus))
	{

		sprintf_s(temp, "SetServiceStatus failed, error code: %d\n", GetLastError());
		trace(temp);
	}
}
