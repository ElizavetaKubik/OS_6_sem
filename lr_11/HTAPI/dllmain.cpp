#include <Windows.h>
#include <iostream>

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::cout << "\nHT DLL PROCESS ATTACH\n" << std::endl;
        break;
    case DLL_THREAD_ATTACH:
        std::cout << "\nHT DLL THREAD ATTACH\n" << std::endl;
        break;
    case DLL_THREAD_DETACH:
        std::cout << "\nHT DLL THREAD DETACH\n" << std::endl;
        break;
    case DLL_PROCESS_DETACH:
        std::cout << "\nHT DLL PROCESS DETACH\n" << std::endl;
        break;
    }
    return TRUE;
}

