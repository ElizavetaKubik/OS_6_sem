#pragma once
#include <string>
#include "Element.h"
using namespace std;

#define DEFAULT_FILE_NAME "map.txt"
#define ARRAY_MAX_SIZE 256

namespace HT
{
	class HashTable
	{
	public:

		HashTable(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName);

		/**
		* Constructor of HashTable, Opens HashTable which is already displayed on the memory
		* need only the name of opened file
		*/
		HashTable(string inFileName);

		/**
		* Constructor of HashTable, Starts HashTable from file
		* need only the name of file, HashTable will take all fields values from there
		*/
		HashTable(const char* inFileName);

		~HashTable();

		int GetCapacity() const;

		int GetCurrentSize() const;

		int GetSnapShotInterval() const;

		void ChangeSnapShotInterval(const int inSnapShotIntervalInSec);

		int GetMaxKeyLength() const;

		void SetMaxKeyLength(const int inMaxKeyLength);

		int GetMaxValueLength() const;

		void SetMaxValueLength(const int inMaxValueLength);

		void* GetElementsStartAddress() const;

		time_t GetLastSnapShotTime() const;

		const char* GetLastErrorMessage() const;

		wstring GetFileName() const;

		bool Insert(const char* inKey, int inKeyLength, const char* inValue, int inValueLength);

		bool Delete(const char* inElementKey);

		void Clear();

		const Element Get(const char* inElementKey);

		bool Update(const char* inOldKey, const char* inNewValue, int inNewValueLength);

		int PrintAll();

		void Print(const char* inFileName);

		bool SaveToFile(const char* inElementKey);

		bool Snap();

		bool Close();

	protected:

		void SetLastErrorMessage(const char* inErrorMessage);

	private:

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

		HashTableData* m_Data;

		Element* ConvertPointerToElement(byte* inPointer);

		byte* GetFreeMemoryForElement();

		int GetAllocatedMemorySizeForHashTable(bool inIncludeConfigData = true);

		byte* FindElement(const char* inKey);

		byte* GetMemorySpaceByIndex(int inIndex);

		friend DWORD WINAPI PeriodicSnapShot(LPVOID inHashTable);
	};

	DWORD WINAPI PeriodicSnapShot(LPVOID inHashTable);
};
