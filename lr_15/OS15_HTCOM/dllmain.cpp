#include <windows.h>
#include <fstream>
#include <objbase.h>
#include "IHashTable.h"
#include "SEQLOG.h"

HMODULE hmodule;
LONG Seq = 0;
std::fstream LogCOM;
LONG SeqHT = 0;
std::fstream LogHT;

ULONG g_Components = 0;
ULONG g_ServerLocks = 0;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hmodule = hModule;
        LogCOM.open(".\\logs\\COM_log.txt", std::ios_base::out);
        LogHT.open(".\\logs\\HT_log.txt", std::ios_base::out);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        LogCOM.close();
        LogHT.close();
        break;
    }
    return TRUE;
}

