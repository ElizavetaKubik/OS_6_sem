#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#include <iostream>

#include "../OS13_HTAPI/Utilities.h"
#include "../OS13_HTAPI/HashTable.h"
#include "../OS13_HTAPI/HashTableAPI.h"

#pragma comment(lib,"../Debug/OS13_HTAPI.lib")

using namespace std;

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

int main(int argc, char* argv[])
{
    if (HTAPI::OpenApi()) {
        HT::HashTableData* HT;
        HT = HTAPI::Open("1.txt");
        if (HT == NULL) {
            printf("Not started\n");
            return -1;
        }

        HTAPI::Snap(HT);
        killProcessByName(L"OS13_Start.exe");
        killProcessByName(L"OS13_Insert.exe");
        killProcessByName(L"OS13_Delete.exe");
        killProcessByName(L"OS13_Update.exe");

        if (HTAPI::CloseApi() == false)
            printf("error with close api\n");
    }
    else {
        printf("error with open api\n");
    }

    return 0;
}