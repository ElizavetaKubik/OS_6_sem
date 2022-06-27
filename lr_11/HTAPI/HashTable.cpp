#include <Windows.h>
#include "HashTable.h"
#include <iostream>
#include <algorithm>
#include "Utilities.h"

#define SECOND 1000


namespace HT   
{
#pragma region InnerFunctions

	byte* GetMemorySpaceByIndex(HashTableData* inHashTableData, int inIndex)
	{
		return (byte*)inHashTableData->GetElementsBeginingAddress() + inIndex * (sizeof(Element) + inHashTableData->MaxKeyLength + inHashTableData->MaxValueLength);
	}

	Element* ConvertPointerToElement(HashTableData* inHashTableData, byte* inPointer)
	{
		Element* result = (Element*)inPointer;
		ElementAPI::SetKeyPointer(result, (char*)(inPointer + sizeof(Element)));
		ElementAPI::SetValuePointer(result, (char*)(inPointer + sizeof(Element) + inHashTableData->MaxKeyLength));
		return result;
	}

	byte* GetFreeMemoryForElement(HashTableData* inHashTableData)
	{
		byte* pointer;
		int count_of_elements = 0;
		Element* temp_el;
		while (count_of_elements < inHashTableData->Capacity)
		{
			pointer = GetMemorySpaceByIndex(inHashTableData, count_of_elements);
			temp_el = (Element*)pointer;
			if (temp_el->KeyLength == 0)
			{
				return pointer;
			}

			count_of_elements++;
		}
		return NULL;
	}

	int GetAllocatedMemorySizeForHashTable(HashTableData* inHashTableData, bool inIncludeConfigData)
	{
		int config_size = inIncludeConfigData ? sizeof(HashTableData) : 0;
		return config_size + (inHashTableData->Capacity) * (sizeof(Element) + inHashTableData->MaxKeyLength + inHashTableData->MaxValueLength);
	}

	byte* FindElement(HashTableData* inHashTableData, const char* inKey)
	{
		byte* pointer;
		int count_of_elements = 0;
		Element* tempEl;
		while (count_of_elements < inHashTableData->Capacity)
		{
			pointer = GetMemorySpaceByIndex(inHashTableData, count_of_elements);
			tempEl = ConvertPointerToElement(inHashTableData, pointer);
			if (tempEl->KeyLength != 0 && strcmp(inKey, tempEl->Key) == 0)
			{
				return pointer;
			}

			count_of_elements++;
		}
		return NULL;
	}

#pragma endregion

#pragma region snap shots


	DWORD WINAPI PeriodicSnapShot(LPVOID inHashTableData)
	{
		HashTableData* hash_table_data = (HashTableData*)inHashTableData;
		if (hash_table_data != nullptr)
		{
			if (time(NULL) >= hash_table_data->LastSnapShotTime + hash_table_data->SnapShotIntervalInSec)
			{
				hash_table_data->TimerHandler = CreateWaitableTimer(NULL, false, hash_table_data->TimerName);
				if (hash_table_data->TimerHandler == NULL)
				{
					return 0;
				}

				long long life_time = -(hash_table_data->SnapShotIntervalInSec * SECOND * SECOND);
				if (SetWaitableTimer(hash_table_data->TimerHandler, (LARGE_INTEGER*)&life_time, hash_table_data->SnapShotIntervalInSec * SECOND, NULL, NULL, false))
				{
					while (hash_table_data != nullptr)
					{
						WaitForSingleObject(hash_table_data->TimerHandler, INFINITE);
						printf("\nasync snapping...\n");
						if (hash_table_data != nullptr)
						{
							Snap(hash_table_data);
						}

					}
				}
			}
		}
		return 0;
	}

	bool Snap(HashTableData* inHashTableData)
	{
		bool snapshot_success = true;

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			int total_ht_size = GetAllocatedMemorySizeForHashTable(inHashTableData);
			if (FlushViewOfFile(inHashTableData, total_ht_size))
			{
				//getting time
				time(&inHashTableData->LastSnapShotTime);
				char* buffer = new char[250];
				ctime_s(buffer, 250, &inHashTableData->LastSnapShotTime);

				//formatting time
				std::tm* current_time = new tm();
				localtime_s(current_time, &inHashTableData->LastSnapShotTime);
				strftime(buffer, 32, "_%d_%m_%Y__%H_%M_%S", current_time);
				delete current_time;
				printf("\nSync Snap at %s\n\n", buffer);

				//creating new file's path
				wstring new_file_snap = GetFileName(inHashTableData);
				new_file_snap.replace(new_file_snap.find('.'), 0, Utilities::ConvertStringToWstring(buffer));

				delete[] buffer;

				//creating new snap file 
				HANDLE file_handler = NULL;
				file_handler = CreateFile(new_file_snap.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (file_handler == INVALID_HANDLE_VALUE)
				{
					ReleaseMutex(mutex_handler);
					SetAndPrintLastErrorMessage(inHashTableData, "Snap: fail with snap file opening");
					return false;
				}
				else
				{
					//writing memory into created snap file 
					DWORD written = -1;
					if (!WriteFile(file_handler, inHashTableData, total_ht_size, &written, NULL))
					{
						ReleaseMutex(mutex_handler);
						SetAndPrintLastErrorMessage(inHashTableData, "Snap: fail with snap file writing");
						return false;
					}
				}

				ReleaseMutex(mutex_handler);
				return snapshot_success;
			}
			else
			{
				ReleaseMutex(mutex_handler);
				SetAndPrintLastErrorMessage(inHashTableData, "Snap: fail with view flushing");
				return false;
			}
		}
		else
		{
			SetAndPrintLastErrorMessage(inHashTableData, "Snap: fail with mutex creation");
			return false;
		}
		return true;
	}

#pragma endregion

	HashTableData::HashTableData(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName,
		HANDLE inFileHandler, HANDLE inFileMappingHandler,
		wstring inFileMapName, wstring inTimerName, wstring inMutexName)
	{
		Capacity = inCapacity;
		SnapShotIntervalInSec = inSecSnapshotInterval;
		MaxKeyLength = inMaxKeyLength;
		MaxValueLength = inMaxValueLength;
		LastSnapShotTime = 0;
		CurrentSize = 0;

		FromStringToToWCharDestination(FileName, inFileName);

		CreatorPid = GetCurrentProcessId();

		TimerHandler = NULL;
		TimerThreadHandler = NULL;

		FileHandler = inFileHandler;
		FileMappingHandler = inFileMappingHandler;

		CopyWcharsFromWStringToDestination(FileMapName, inFileMapName);
		CopyWcharsFromWStringToDestination(FileMapName, inFileMapName);
		CopyWcharsFromWStringToDestination(TimerName, inTimerName);
		CopyWcharsFromWStringToDestination(MutexName, inMutexName);

	}

	HashTableData* CreateHT(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName)
	{		
		HANDLE file_handler = CreateFile(Utilities::ConvertStringToWstring(inFileName).c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_handler == INVALID_HANDLE_VALUE)
		{
			cout << "specified file is already opened, map is not able to be displayed";
			return nullptr;
		}

		//file should have at least 1 symbol, cuz CreateFileMapping function needed it
		DWORD written;
		WriteFile(file_handler, " ", 1, &written, NULL);

		int ht_size = sizeof(HashTableData) + inCapacity * (sizeof(Element) + inMaxKeyLength + inMaxValueLength);

		wstring general_prefix_name = Utilities::ConvertStringToWstring(inFileName);
		std::replace(general_prefix_name.begin(), general_prefix_name.end(), '\\', '_');

		wstring file_map_name = general_prefix_name + L"_map";
		wstring timer_name = general_prefix_name + L"_timer";
		wstring mutex_name = general_prefix_name + L"_mutex";

		HANDLE file_mapping_handler = CreateFileMapping(file_handler, NULL, PAGE_READWRITE, 0, ht_size, file_map_name.c_str());
		if (!file_mapping_handler)
		{
			cout << "create filemapping error";
			return nullptr;
		}

		LPVOID map_view_pointer = MapViewOfFile(file_mapping_handler, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (!map_view_pointer)
		{
			cout << "mapping file error";
			return nullptr;
		}
		ZeroMemory(map_view_pointer, ht_size);

		HashTableData* ht_data = new(map_view_pointer) HashTableData(inCapacity, inSecSnapshotInterval, inMaxKeyLength, inMaxValueLength, inFileName,
			file_handler, file_mapping_handler,
			file_map_name, timer_name, mutex_name);

		DWORD timer_thread_id;
		ht_data->TimerThreadHandler = CreateThread(NULL, NULL, HT::PeriodicSnapShot, ht_data, NULL, &timer_thread_id);
		if (ht_data->TimerThreadHandler == NULL)
		{
			cout << "creating snap shot thread error";
			return nullptr;
		}

		return ht_data;
	}

	HashTableData* OpenHT_FromFile(string inFileName)
	{
		wstring general_prefix_name = Utilities::ConvertStringToWstring(inFileName);
		std::replace(general_prefix_name.begin(), general_prefix_name.end(), '\\', '_');

		wstring file_map_name = general_prefix_name + L"_map";

		HANDLE file_mapping_handler = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, file_map_name.c_str());
		if (file_mapping_handler == nullptr)
		{
			cout << "\n error in HashTable constructor for opening: create filemapping error\n";
			return nullptr;
		}

		LPVOID map_view_of_file = MapViewOfFile(file_mapping_handler, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (file_mapping_handler == nullptr)
		{
			cout << "\n error in HashTable constructor for opening: view of file is not accessable\n";
			return nullptr;
		}

		HashTableData* ht_data = reinterpret_cast<HashTableData*>(map_view_of_file);
		if (ht_data == nullptr)
		{
			cout << "\n error in HashTable constructor for starting: can't read data of Hash Table\n";
			return nullptr;
		}

		return ht_data;
	}

	HashTableData* CreateHT_FromFile(const char* inFileName)
	{
		HANDLE file_handler = CreateFile(Utilities::ConvertStringToWstring(string(inFileName)).c_str(), GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_handler == INVALID_HANDLE_VALUE)
		{
			cout << "specified file is already opened, map is not able to be displayed";
			return nullptr;
		}

		wstring general_prefix_name = Utilities::ConvertStringToWstring(inFileName);
		std::replace(general_prefix_name.begin(), general_prefix_name.end(), '\\', '_');

		wstring file_map_name = general_prefix_name + L"_map";

		HANDLE file_mapping_handler = CreateFileMapping(file_handler, NULL, PAGE_READWRITE, 0, 0, file_map_name.c_str());
		if (!file_mapping_handler)
		{
			cout << "\n error in HashTable constructor for starting: create filemapping error\n";
			return nullptr;
		}

		LPVOID map_view_of_file = MapViewOfFile(file_mapping_handler, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (file_mapping_handler == nullptr)
		{
			cout << "\n error in HashTable constructor for starting: view of file is not accessable\n";
			return nullptr;
		}

		HashTableData* ht_data = reinterpret_cast<HashTableData*>(map_view_of_file);
		if (ht_data == nullptr)
		{
			cout << "\n error in HashTable constructor for opening: can't read data of Hash Table\n";
			return nullptr;
		}

		ht_data->FileHandler = file_handler;
		ht_data->FileMappingHandler = file_mapping_handler;

		DWORD timer_thread_id;
		ht_data->TimerThreadHandler = CreateThread(NULL, NULL, HT::PeriodicSnapShot, ht_data, NULL, &timer_thread_id);
		if (ht_data->TimerThreadHandler == NULL)
		{
			cout << "Creating snap shot thread error";
			return nullptr;
		}
		return ht_data;
	}

#pragma region Getters and Setters
	int GetCapacity(HashTableData* inHashTableData)
	{
		return inHashTableData->Capacity;
	}

	int GetCurrentSize(HashTableData* inHashTableData)
	{
		return inHashTableData->CurrentSize;
	}

	int GetSnapShotInterval(HashTableData* inHashTableData)
	{
		return inHashTableData->SnapShotIntervalInSec;
	}

	void ChangeSnapShotInterval(HashTableData* inHashTableData, const int inSnapShotIntervalInSec)
	{
		inHashTableData->SnapShotIntervalInSec = inSnapShotIntervalInSec;
	}

	int GetMaxKeyLength(HashTableData* inHashTableData)
	{
		return inHashTableData->MaxKeyLength;
	}

	void SetMaxKeyLength(HashTableData* inHashTableData, const int inMaxKeyLength)
	{
		inHashTableData->MaxKeyLength = inMaxKeyLength;
	}

	int GetMaxValueLength(HashTableData* inHashTableData)
	{
		return inHashTableData->MaxValueLength;
	}

	void SetMaxValueLength(HashTableData* inHashTableData, const int inMaxValueLength)
	{
		inHashTableData->MaxValueLength = inMaxValueLength;
	}

	void* GetElementsStartAddress(HashTableData* inHashTableData)
	{
		return inHashTableData->GetElementsBeginingAddress();
	}

	time_t GetLastSnapShotTime(HashTableData* inHashTableData)
	{
		return inHashTableData->LastSnapShotTime;
	}

	const char* GetLastErrorMessage(HashTableData* inHashTableData)
	{
		return inHashTableData->LastErrorMessage;
	}

	wstring GetFileName(HashTableData* inHashTableData)
	{
		return wstring(inHashTableData->FileName);
	}

	void SetAndPrintLastErrorMessage(HashTableData* inHashTableData, const char* inErrorMessage)
	{
		if (inHashTableData != nullptr)
		{
			strcpy_s(inHashTableData->LastErrorMessage, inErrorMessage);
			cout << inErrorMessage;
		}
	}

	void HashTableData::FromStringToToWCharDestination(wchar_t* inDestination, string inString)
	{
		wstring temp_file_name = Utilities::ConvertStringToWstring(inString);

		for (unsigned int i = 0; i < temp_file_name.length() && i < ARRAY_MAX_SIZE; i++)
		{
			inDestination[i] = temp_file_name.at(i);
		}
	}

	void HashTableData::CopyWcharsFromWStringToDestination(wchar_t* inDestination, wstring inWString)
	{

		for (unsigned int i = 0; i < inWString.length() && i < ARRAY_MAX_SIZE; i++)
		{
			inDestination[i] = inWString.at(i);
		}
	}

	byte* HashTableData::GetElementsBeginingAddress()
	{
		return (byte*)this + sizeof(HashTableData);
	}
#pragma endregion

	bool Close(HashTableData* inHashTableData)
	{
		if (inHashTableData == NULL)
		{
			cout << "hash table is not opened\n";
			return false;
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			if (inHashTableData->CreatorPid == GetCurrentProcessId())
			{
				if (inHashTableData->FileMappingHandler == NULL || !CloseHandle(inHashTableData->FileMappingHandler))
				{
					ReleaseMutex(mutex_handler);
					SetAndPrintLastErrorMessage(inHashTableData, "Close: close file mapping");
					return false;
				}

				if (inHashTableData->FileHandler == NULL || !CloseHandle(inHashTableData->FileHandler))
				{
					ReleaseMutex(mutex_handler);
					SetAndPrintLastErrorMessage(inHashTableData, "Close: close file handler");
					return false;
				}

				if (inHashTableData->TimerHandler != NULL)
				{
					if (!CloseHandle(inHashTableData->TimerHandler))
					{
						ReleaseMutex(mutex_handler);
						SetAndPrintLastErrorMessage(inHashTableData, "Close: close timer handler");
						return false;
					}
				}

				inHashTableData->FileHandler = NULL;
				inHashTableData->FileMappingHandler = NULL;
				inHashTableData->TimerHandler = NULL;
				inHashTableData->TimerThreadHandler = NULL;
			}
			else
			{
				ReleaseMutex(mutex_handler);
			}
		}
		else
		{
			if (mutex_handler != NULL)
			{
				ReleaseMutex(mutex_handler);
			}
			SetAndPrintLastErrorMessage(inHashTableData, "Close: fail with mutex creation");
			return false;
		}

		ReleaseMutex(mutex_handler);

		if (!UnmapViewOfFile(inHashTableData->GetElementsBeginingAddress()))
		{
			SetAndPrintLastErrorMessage(inHashTableData, "Close: unmapping file error");
			return false;
		}
		inHashTableData = NULL;
		return true;
	}


	bool Insert(HashTableData* inHashTableData, const char* inKey, int inKeyLength, const char* inValue, int inValueLength)
	{
		if (inHashTableData == nullptr)
		{
			cout << "hash table isn't opened, operation is cancelled";
			return false;
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);

		if (mutex_handler)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inHashTableData, inKey);
			if (pointer != NULL)
			{
				ReleaseMutex(mutex_handler);
				SetAndPrintLastErrorMessage(inHashTableData, "Insert - Hash Table already have such element");
				return false;
			}
			pointer = GetFreeMemoryForElement(inHashTableData);
			if (pointer == NULL)
			{
				ReleaseMutex(mutex_handler);
				SetAndPrintLastErrorMessage(inHashTableData, "Insert - can't give free memory range for new element");
				return false;
			}
			else
			{
				Element* new_element = ConvertPointerToElement(inHashTableData, pointer);
				string temp_string;
				temp_string = inKey;


				int new_size = min(inKeyLength, inHashTableData->MaxKeyLength);
				temp_string.resize(new_size);
				ElementAPI::ClearKey(new_element);
				ElementAPI::SetKey(new_element, temp_string.c_str(), temp_string.length());

				temp_string = inValue;
				new_size = min(inValueLength, inHashTableData->MaxKeyLength);
				temp_string.resize(new_size);
				ElementAPI::ClearValue(new_element);
				ElementAPI::SetValue(new_element, temp_string.c_str(), temp_string.length());

				ReleaseMutex(mutex_handler);
				inHashTableData->CurrentSize++;
			}
		}
		else
		{
			SetAndPrintLastErrorMessage(inHashTableData, "Insert: fail with mutex creation");
			return false;
		}

		return true;
	}

	bool Delete(HashTableData* inHashTableData, const char* inElementKey)
	{
		if (inHashTableData == nullptr)
		{
			cout << "hash table isn't opened, operation is cancelled";
			return false;
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);
		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inHashTableData, inElementKey);
			if (pointer == NULL)
			{
				ReleaseMutex(mutex_handler);
				SetAndPrintLastErrorMessage(inHashTableData, "Delete - have not found such element");
				return false;
			}
			else
			{
				ZeroMemory(pointer, sizeof(Element) + inHashTableData->MaxKeyLength + inHashTableData->MaxValueLength);
				ReleaseMutex(mutex_handler);
				inHashTableData->CurrentSize--;
				return true;
			}
		}
		else
		{
			SetAndPrintLastErrorMessage(inHashTableData, "Delete: fail with mutex creation");
			return false;
		}
	}

	const Element Get(HashTableData* inHashTableData, const char* inElementKey)
	{
		if (inHashTableData == nullptr)
		{
			cout << "hash table isn't opened, operation is cancelled";
			return {};
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inHashTableData, inElementKey);
			if (pointer == NULL)
			{
				ReleaseMutex(mutex_handler);
				SetAndPrintLastErrorMessage(inHashTableData, "Get - have not found such element");
				return {};
			}
			else
			{
				Element* result = ConvertPointerToElement(inHashTableData, pointer);
				ReleaseMutex(mutex_handler);
				return *result;
			}
		}
		else
		{
			SetAndPrintLastErrorMessage(inHashTableData, "Get: fail with mutex creation");
			return {};
		}
	}

	bool Update(HashTableData* inHashTableData, const char* inOldKey, const char* inNewValue, int inNewValueLength)
	{
		if (inHashTableData == nullptr)
		{
			cout << "hash table isn't opened, operation is cancelled";
			return false;
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inHashTableData, inOldKey);
			if (pointer == NULL)
			{
				SetAndPrintLastErrorMessage(inHashTableData, "Update - there is no such element for updating");
				ReleaseMutex(mutex_handler);
				return false;
			}
			else
			{
				Element* element = ConvertPointerToElement(inHashTableData, pointer);

				string temp_string;
				temp_string = inNewValue;

				int new_size = min(inNewValueLength, inHashTableData->MaxValueLength);
				temp_string.resize(new_size);
				ElementAPI::ClearValue(element);
				ElementAPI::SetValue(element, temp_string.c_str(), temp_string.length());

				ReleaseMutex(mutex_handler);
			}
		}
		else
		{
			SetAndPrintLastErrorMessage(inHashTableData, "Update: fail with mutex creation");
			return false;
		}

		return true;
	}

	void Clear(HashTableData* inHashTableData)
	{
		if (inHashTableData == nullptr)
		{
			cout << "hash table isn't opened, operation is cancelled";
			return;
		}

		int totalHtSize = GetAllocatedMemorySizeForHashTable(inHashTableData, false);
		ZeroMemory(inHashTableData->GetElementsBeginingAddress(), totalHtSize);
		inHashTableData->CurrentSize = 0;
	}

	int PrintAll(HashTableData* inHashTableData)
	{
		if (inHashTableData == nullptr)
		{
			cout << "hash table isn't opened, operation is cancelled";
			return -1;
		}
		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer;
			int count_of_elements = 0;
			Element* temp_el;

			while (count_of_elements < inHashTableData->Capacity)
			{
				pointer = GetMemorySpaceByIndex(inHashTableData, count_of_elements);
				temp_el = ConvertPointerToElement(inHashTableData, pointer);

				if (temp_el->KeyLength != 0)
				{
					Print(inHashTableData, temp_el->Key);
					cout << endl;
				}
				count_of_elements++;
			}

			ReleaseMutex(mutex_handler);

			if (count_of_elements == 0)
			{
				cout << "Hash Table is empty\n";
				return -1;
			}

			return count_of_elements;
		}
		else
		{
			SetAndPrintLastErrorMessage(inHashTableData, "PrintAll: fail with mutex creation");
			return -1;
		}

	}

	void Print(HashTableData* inHashTableData, const char* inElementKey)
	{
		if (inHashTableData == nullptr)
		{
			cout << "hash table isn't opened, operation is cancelled";
			return;
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);

		if (mutex_handler)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inHashTableData, inElementKey);
			if (pointer != NULL)
			{
				Element* result = ConvertPointerToElement(inHashTableData, pointer);
				printf("key: %s, value: %s", result->Key, result->Value);
			}
			else
			{
				printf("element with key %s is not found", inElementKey);
			}

			ReleaseMutex(mutex_handler);
		}
		else
		{
			SetAndPrintLastErrorMessage(inHashTableData, "PrintAll: fail with mutex creation");
			return;
		}

	}

	bool SaveToFile(HashTableData* inHashTableData, const char* inFileName)			
	{
		if (inHashTableData == nullptr)
		{
			cout << "hash table isn't opened, operation is cancelled";
			return false;
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, inHashTableData->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			int totalHtSize = GetAllocatedMemorySizeForHashTable(inHashTableData);
			if (FlushViewOfFile(inHashTableData->GetElementsBeginingAddress(), totalHtSize))
			{
				wstring save_file_path = Utilities::ConvertStringToWstring(Utilities::GetModulePath() + string(inFileName));

				//creating new save file 
				HANDLE file_handler = NULL;
				file_handler = CreateFile(save_file_path.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (file_handler == INVALID_HANDLE_VALUE)
				{
					SetAndPrintLastErrorMessage(inHashTableData, "SaveToFile: fail with save file creating for hash table");
					ReleaseMutex(mutex_handler);
					return false;
				}
				else
				{
					//writing memory into created save file 
					DWORD written = -1;
					if (!WriteFile(file_handler, inHashTableData, totalHtSize, &written, NULL))
					{
						SetAndPrintLastErrorMessage(inHashTableData, "SaveToFile: fail with writing data to the save file");
						ReleaseMutex(mutex_handler);
						return false;
					}
				}

				ReleaseMutex(mutex_handler);
				return true;
			}
			else
			{
				ReleaseMutex(mutex_handler);
				return false;
			}
		}
		else
		{
			SetAndPrintLastErrorMessage(inHashTableData, "PrintAll: fail with mutex creation");
			return false;
		}
	}
};
