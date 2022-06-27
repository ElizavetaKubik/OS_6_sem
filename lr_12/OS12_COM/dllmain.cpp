#include <windows.h>
#include <fstream>
#include <objbase.h>
#include "Interface.h"
#include "Registry.h"
#include "SEQLOG.h"

HMODULE hmodule;
LONG Seq = 0;
std::fstream LogCOM;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hmodule = hModule;
		LogCOM.open("d:\\logCOM.txt", std::ios_base::out);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH: LogCOM.close(); break;
		break;
	}
	return TRUE;
}


STDAPI DllInstall(BOOL b, PCWSTR s) {
	SEQ;
	LOG("DllInstall ", (b) ? "install" : "uninstall");
	return S_OK;
}
STDAPI DllRegisterServer()
{
	SEQ;
	LOG("DllRegisterServer ", hmodule);
	return RegisterServer(
		hmodule,
		CLSID_OS12,
		FNAME,
		VERS,
		PGID
	);
}
STDAPI DllUnregisterServer()
{
	SEQ;
	LOG("DllUnregister ", hmodule);
	return UnregisterServer(
		CLSID_OS12,
		VERS,
		PGID
	);
}

// {1347EB8D-0D92-4D69-A1DD-24EC00C6EDCC}
//DEFINE_GUID(<< name >> ,
//    0x1347eb8d, 0xd92, 0x4d69, 0xa1, 0xdd, 0x24, 0xec, 0x0, 0xc6, 0xed, 0xcc);

// {4D572117-F203-4386-A54F-8A204848E250}
//DEFINE_GUID(<< name >> ,
//    0x4d572117, 0xf203, 0x4386, 0xa5, 0x4f, 0x8a, 0x20, 0x48, 0x48, 0xe2, 0x50);

// {B852F60C-EA1C-4843-9B96-16C75F9FC1EE}
//DEFINE_GUID(<< name >> ,
//    0xb852f60c, 0xea1c, 0x4843, 0x9b, 0x96, 0x16, 0xc7, 0x5f, 0x9f, 0xc1, 0xee);