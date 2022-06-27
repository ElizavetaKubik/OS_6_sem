#pragma once
#include <Windows.h>
#include <string>
#include "Element.h"
using namespace std;

#define DEFAULT_FILE_NAME "map.txt"
#define ARRAY_MAX_SIZE 256

#define HT_API extern "C" __declspec(dllexport)

namespace HT   
{
	struct HashTableData
	{
		HashTableData(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName,
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

		DWORD	CreatorPid;

		HANDLE  FileHandler;
		HANDLE  FileMappingHandler;
		HANDLE	TimerHandler;
		HANDLE	TimerThreadHandler;
	};

	HT_API HashTableData* CreateHT(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName);

	HT_API HashTableData* OpenHT_FromFile(string inFileName);

	HT_API HashTableData* CreateHT_FromFile(const char* inFileName);

	HT_API const char* GetLastErrorMessage(HashTableData* inHT_Data);

	HT_API bool Insert(HashTableData* inHT_Data, const char* inKey, int inKeyLength, const char* inValue, int inValueLength);

	HT_API bool Delete(HashTableData* inHT_Data, const char* inElementKey);

	HT_API Element* Get(HashTableData* inHT_Data, const char* inElementKey);

	HT_API bool Update(HashTableData* inHT_Data, const char* inOldKey, const char* inNewValue, int inNewValueLength);

	HT_API bool Snap(HashTableData* inHT_Data);

	HT_API bool Close(HashTableData* inHT_Data);
};

