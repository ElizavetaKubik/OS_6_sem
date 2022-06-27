#pragma once
#include <string>
#include "Element.h"
#include <Windows.h>
using namespace std;

#define DEFAULT_FILE_NAME "map.txt"
#define ARRAY_MAX_SIZE 256

#define HT_API extern "C" __declspec(dllexport)

namespace HT
{
	struct HashTableData
	{
		HashTableData(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName,
			LPCWSTR HTUsersGroup,
			HANDLE inFileHandler, HANDLE inFileMappingHandler,
			wstring inFileMapName, wstring inTimerName, wstring inMutexName);

		static void FromStringToToWCharDestination(wchar_t* inDestination, string inString);

		static void CopyWcharsFromWStringToDestination(wchar_t* inDestination, wstring inWString);

		byte* GetElementsBeginingAddress();

		int     CurrentSize;
		int     Capacity;
		int     SnapShotIntervalInSec;
		int     MaxKeyLength;
		int     MaxValueLength;
		time_t  LastSnapShotTime;

		char  LastErrorMessage[ARRAY_MAX_SIZE];

		wchar_t MutexName[ARRAY_MAX_SIZE];
		wchar_t	FileName[ARRAY_MAX_SIZE];
		wchar_t FileMapName[ARRAY_MAX_SIZE];
		wchar_t TimerName[ARRAY_MAX_SIZE];
		wchar_t HTUsersGroup[ARRAY_MAX_SIZE];

		DWORD	CreatorPid;

		HANDLE  FileHandler;
		HANDLE  FileMappingHandler;
		HANDLE	TimerHandler;
		HANDLE	TimerThreadHandler;
	};

	HT_API HashTableData* CreateHT(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName, LPCWSTR HTUsersGroup);

	HT_API HashTableData* OpenHT_FromFile(string inFileName);

	HT_API HashTableData* CreateHT_FromFile(const char* inFileName);

	__declspec(dllexport) HashTableData* Open(string inFileName);

	__declspec(dllexport) HashTableData* Open(string inFileName, LPCWSTR userName, LPCWSTR password);

	HT_API int GetCapacity(HashTableData* inHT_Data);

	HT_API int GetCurrentSize(HashTableData* inHT_Data);

	HT_API int GetSnapShotInterval(HashTableData* inHT_Data);

	HT_API void ChangeSnapShotInterval(HashTableData* inHT_Data, const int inSnapShotIntervalInSec);

	HT_API int GetMaxKeyLength(HashTableData* inHT_Data);

	HT_API void SetMaxKeyLength(HashTableData* inHT_Data, const int inMaxKeyLength);

	HT_API int GetMaxValueLength(HashTableData* inHT_Data);

	HT_API void SetMaxValueLength(HashTableData* inHT_Data, const int inMaxValueLength);

	HT_API void* GetElementsStartAddress(HashTableData* inHT_Data);

	HT_API time_t GetLastSnapShotTime(HashTableData* inHT_Data);

	HT_API const char* GetLastErrorMessage(HashTableData* inHT_Data);

	HT_API wstring GetFileName(HashTableData* inHT_Data);

	HT_API bool Insert(HashTableData* inHT_Data, const char* inKey, int inKeyLength, const char* inValue, int inValueLength);

	HT_API bool Delete(HashTableData* inHT_Data, const char* inElementKey);

	HT_API void Clear(HashTableData* inHT_Data);

	HT_API Element* Get(HashTableData* inHT_Data, const char* inElementKey);

	HT_API bool Update(HashTableData* inHT_Data, const char* inOldKey, const char* inNewValue, int inNewValueLength);

	HT_API int PrintAll(HashTableData* inHT_Data);

	HT_API void Print(HashTableData* inHT_Data, const char* inFileName);

	HT_API bool SaveToFile(HashTableData* inHT_Data, const char* inElementKey);

	DWORD WINAPI PeriodicSnapShot(LPVOID* inHT_Data);

	HT_API bool Snap(HashTableData* inHT_Data);

	HT_API bool Close(HashTableData* inHT_Data);

	HT_API void SetAndPrintLastErrorMessage(HashTableData* inHT_Data, const char* inErrorMessage);

	HT_API Element* ConvertPointerToElement(HashTableData* inHT_Data, byte* inPointer);

	HT_API byte* GetFreeMemoryForElement(HashTableData* inHT_Data);

	HT_API int GetAllocatedMemorySizeForHashTable(HashTableData* inHT_Data, bool inIncludeConfigData = true);

	HT_API byte* FindElement(HashTableData* inHT_Data, const char* inKey);

	HT_API byte* GetMemorySpaceByIndex(HashTableData* inHT_Data, int inIndex);
};
