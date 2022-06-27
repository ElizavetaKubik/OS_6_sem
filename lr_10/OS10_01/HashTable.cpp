#include <Windows.h>
#include "HashTable.h"
#include <iostream>
#include <algorithm>
#include "Utilities.h"

#define SECOND 1000


namespace HT
{
#pragma region InnerFunctions

	byte* HashTable::GetMemorySpaceByIndex(int inIndex)
	{
		return (byte*)m_Data->GetElementsBeginingAddress() + inIndex * (sizeof(Element) + m_Data->MaxKeyLength + m_Data->MaxValueLength);
	}

	Element* HashTable::ConvertPointerToElement(byte* inPointer)
	{
		Element* result = (Element*)inPointer;
		result->SetKeyPointer((char*)(inPointer + sizeof(Element)));
		result->SetValuePointer((char*)(inPointer + sizeof(Element) + m_Data->MaxKeyLength));
		return result;
	}

	byte* HashTable::GetFreeMemoryForElement()
	{
		byte* pointer;
		int count_of_elements = 0;
		Element* temp_el;
		while (count_of_elements < m_Data->Capacity)
		{
			pointer = GetMemorySpaceByIndex(count_of_elements);
			temp_el = (Element*)pointer;
			if (temp_el->GetKeyLength() == 0)
			{
				return pointer;
			}

			count_of_elements++;
		}
		return NULL;
	}

	int HashTable::GetAllocatedMemorySizeForHashTable(bool inIncludeConfigData)
	{
		int config_size = inIncludeConfigData ? sizeof(HashTableData) : 0;
		return config_size + (m_Data->Capacity) * (sizeof(Element) + m_Data->MaxKeyLength + m_Data->MaxValueLength);
	}

	byte* HashTable::FindElement(const char* inKey)
	{
		byte* pointer;
		int count_of_elements = 0;
		Element* tempEl;
		while (count_of_elements < m_Data->Capacity)
		{
			pointer = GetMemorySpaceByIndex(count_of_elements);
			tempEl = ConvertPointerToElement(pointer);
			if (tempEl->GetKeyLength() != 0 && strcmp(inKey, tempEl->GetKey()) == 0)
			{
				return pointer;
			}

			count_of_elements++;
		}
		return NULL;
	}

#pragma endregion

#pragma region snap shots


	DWORD WINAPI HT::PeriodicSnapShot(LPVOID inHashTable)
	{
		HashTable* hash_table = (HashTable*)inHashTable;
		if (hash_table != nullptr && hash_table->m_Data != nullptr)
		{
			if (time(NULL) >= hash_table->m_Data->LastSnapShotTime + hash_table->m_Data->SnapShotIntervalInSec)
			{
				hash_table->m_Data->TimerHandler = CreateWaitableTimer(NULL, false, hash_table->m_Data->TimerName);
				if (hash_table->m_Data->TimerHandler == NULL)
				{
					return 0;
				}

				long long life_time = -(hash_table->m_Data->SnapShotIntervalInSec * SECOND * SECOND);
				if (SetWaitableTimer(hash_table->m_Data->TimerHandler, (LARGE_INTEGER*)&life_time, hash_table->m_Data->SnapShotIntervalInSec * SECOND, NULL, NULL, false))
				{
					while (hash_table != nullptr)
					{
						WaitForSingleObject(hash_table->m_Data->TimerHandler, INFINITE);
						printf("\nasync snapping...\n");
						if (hash_table != nullptr)
						{
							hash_table->Snap();
						}

					}
				}
			}
		}
		return 0;
	}

	bool HashTable::Snap()
	{
		bool snapshot_success = true;

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			int total_ht_size = GetAllocatedMemorySizeForHashTable();
			if (FlushViewOfFile(m_Data, total_ht_size))
			{
				//getting time
				time(&m_Data->LastSnapShotTime);
				char* buffer = new char[250];
				ctime_s(buffer, 250, &m_Data->LastSnapShotTime);

				//formatting time
				std::tm* current_time = new tm();
				localtime_s(current_time, &m_Data->LastSnapShotTime);
				strftime(buffer, 32, "_%d_%m_%Y__%H_%M_%S", current_time);
				delete current_time;
				printf("\nSync Snap at %s\n\n", buffer);

				//creating new file's path
				wstring new_file_snap = this->GetFileName();
				new_file_snap.replace(new_file_snap.find('.'), 0, Utilities::ConvertStringToWstring(buffer));

				delete[] buffer;

				//creating new snap file 
				HANDLE file_handler = NULL;
				file_handler = CreateFile(new_file_snap.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (file_handler == INVALID_HANDLE_VALUE)
				{
					ReleaseMutex(mutex_handler);
					SetLastErrorMessage("HashTable::Snap: fail with snap file opening");
					throw GetLastErrorMessage();
				}
				else
				{
					//writing memory into created snap file 
					DWORD written = -1;
					if (!WriteFile(file_handler, m_Data, total_ht_size, &written, NULL))
					{
						ReleaseMutex(mutex_handler);
						SetLastErrorMessage("HashTable::Snap: fail with snap file writing");
						throw GetLastErrorMessage();
					}
				}

				ReleaseMutex(mutex_handler);
				return snapshot_success;
			}
			else
			{
				ReleaseMutex(mutex_handler);
				SetLastErrorMessage("HashTable::Snap: fail with view flushing");
				throw GetLastErrorMessage();
			}
		}
		else
		{
			SetLastErrorMessage("HashTable::Snap: fail with mutex creation");
			throw GetLastErrorMessage();
		}
		return true;
	}

#pragma endregion

	HashTable::HashTableData::HashTableData(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName,
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

	HashTable::HashTable(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName)
	{
		HANDLE file_handler = NULL;
		HANDLE file_mapping_handler = NULL;
		try
		{
			file_handler = CreateFile(Utilities::ConvertStringToWstring(inFileName).c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file_handler == INVALID_HANDLE_VALUE)
			{
				throw "specified file is already opened, map is not able to be displayed";
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

			file_mapping_handler = CreateFileMapping(file_handler, NULL, PAGE_READWRITE, 0, ht_size, file_map_name.c_str());
			if (!file_mapping_handler)
			{
				throw "create filemapping error";
			}

			LPVOID map_view_pointer = MapViewOfFile(file_mapping_handler, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (!map_view_pointer)
			{
				throw "mapping file error";
			}
			ZeroMemory(map_view_pointer, ht_size);

			m_Data = new(map_view_pointer) HashTableData(inCapacity, inSecSnapshotInterval, inMaxKeyLength, inMaxValueLength, inFileName,
				file_handler, file_mapping_handler,
				file_map_name, timer_name, mutex_name);

			DWORD timer_thread_id;
			m_Data->TimerThreadHandler = CreateThread(NULL, NULL, HT::PeriodicSnapShot, this, NULL, &timer_thread_id);
			if (m_Data->TimerThreadHandler == NULL)
			{
				throw "creating snap shot thread error";
			}
		}
		catch (const char* e)
		{
			cout << "\nerror in HashTable initialization: " << e << std::endl;
			CloseHandle(file_handler);
			CloseHandle(file_mapping_handler);
			SetLastErrorMessage(e);
		}
		catch (...)
		{
			SetLastErrorMessage("\n unhandled error in HashTable::OpenHashTableFiles()\n");
			cout << m_Data->LastErrorMessage;
		}
	}

	HashTable::HashTable(string inFileName)
	{
		wstring general_prefix_name = Utilities::ConvertStringToWstring(inFileName);
		std::replace(general_prefix_name.begin(), general_prefix_name.end(), '\\', '_');

		wstring file_map_name = general_prefix_name + L"_map";

		HANDLE file_mapping_handler = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, file_map_name.c_str());
		if (file_mapping_handler == nullptr)
		{
			throw "\n error in HashTable constructor for opening: create filemapping error\n";
		}

		LPVOID map_view_of_file = MapViewOfFile(file_mapping_handler, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (file_mapping_handler == nullptr)
		{
			throw "\n error in HashTable constructor for opening: view of file is not accessable\n";
		}

		m_Data = reinterpret_cast<HashTableData*>(map_view_of_file);
		if (m_Data == nullptr)
		{
			throw "\n error in HashTable constructor for starting: can't read data of Hash Table\n";
		}

	}

	HashTable::HashTable(const char* inFileName)
	{
		HANDLE file_handler = CreateFile(Utilities::ConvertStringToWstring(string(inFileName)).c_str(), GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_handler == INVALID_HANDLE_VALUE)
		{
			throw "specified file is already opened, map is not able to be displayed";
		}

		wstring general_prefix_name = Utilities::ConvertStringToWstring(inFileName);
		std::replace(general_prefix_name.begin(), general_prefix_name.end(), '\\', '_');

		wstring file_map_name = general_prefix_name + L"_map";

		HANDLE file_mapping_handler = CreateFileMapping(file_handler, NULL, PAGE_READWRITE, 0, 0, file_map_name.c_str());
		if (!file_mapping_handler)
		{
			throw "\n error in HashTable constructor for starting: create filemapping error\n";
		}

		LPVOID map_view_of_file = MapViewOfFile(file_mapping_handler, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (file_mapping_handler == nullptr)
		{
			throw "\n error in HashTable constructor for starting: view of file is not accessable\n";
		}

		m_Data = reinterpret_cast<HashTableData*>(map_view_of_file);
		if (m_Data == nullptr)
		{
			throw "\n error in HashTable constructor for opening: can't read data of Hash Table\n";
		}

		m_Data->FileHandler = file_handler;
		m_Data->FileMappingHandler = file_mapping_handler;

		DWORD timer_thread_id;
		m_Data->TimerThreadHandler = CreateThread(NULL, NULL, HT::PeriodicSnapShot, this, NULL, &timer_thread_id);
		if (m_Data->TimerThreadHandler == NULL)
		{
			throw "Creating snap shot thread error";
		}

	}

	HashTable::~HashTable()
	{

	}

#pragma region Getters and Setters
	int HashTable::GetCapacity() const
	{
		return m_Data->Capacity;
	}

	int HashTable::GetCurrentSize() const
	{
		return m_Data->CurrentSize;
	}

	int HashTable::GetSnapShotInterval() const
	{
		return m_Data->SnapShotIntervalInSec;
	}

	void HashTable::ChangeSnapShotInterval(const int inSnapShotIntervalInSec)
	{
		m_Data->SnapShotIntervalInSec = inSnapShotIntervalInSec;
	}

	int HashTable::GetMaxKeyLength() const
	{
		return m_Data->MaxKeyLength;
	}

	void HashTable::SetMaxKeyLength(const int inMaxKeyLength)
	{
		m_Data->MaxKeyLength = inMaxKeyLength;
	}

	int HashTable::GetMaxValueLength() const
	{
		return m_Data->MaxValueLength;
	}

	void HashTable::SetMaxValueLength(const int inMaxValueLength)
	{
		m_Data->MaxValueLength = inMaxValueLength;
	}

	void* HashTable::GetElementsStartAddress() const
	{
		return m_Data->GetElementsBeginingAddress();
	}

	time_t HashTable::GetLastSnapShotTime() const
	{
		return m_Data->LastSnapShotTime;
	}

	const char* HashTable::GetLastErrorMessage() const
	{
		return m_Data->LastErrorMessage;
	}

	wstring HashTable::GetFileName() const
	{

		return wstring(m_Data->FileName);
	}

	void HashTable::SetLastErrorMessage(const char* inErrorMessage)
	{
		if (m_Data != nullptr)
		{
			strcpy_s(m_Data->LastErrorMessage, inErrorMessage);
		}
	}

	void HashTable::HashTableData::FromStringToToWCharDestination(wchar_t* inDestination, string inString)
	{
		wstring temp_file_name = Utilities::ConvertStringToWstring(inString);

		for (unsigned int i = 0; i < temp_file_name.length() && i < ARRAY_MAX_SIZE; i++)
		{
			inDestination[i] = temp_file_name.at(i);
		}
	}

	void HashTable::HashTableData::CopyWcharsFromWStringToDestination(wchar_t* inDestination, wstring inWString)
	{

		for (unsigned int i = 0; i < inWString.length() && i < ARRAY_MAX_SIZE; i++)
		{
			inDestination[i] = inWString.at(i);
		}
	}

	byte* HashTable::HashTableData::GetElementsBeginingAddress()
	{
		return (byte*)this + sizeof(HashTableData);
	}
#pragma endregion

	bool HashTable::Close()
	{
		HANDLE mutex_handler = NULL;
		try
		{
			if (m_Data == NULL)
			{
				throw "hash table is not opened\n";
			}

			mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);

			if (mutex_handler != NULL)
			{
				WaitForSingleObject(mutex_handler, INFINITE);

				if (m_Data->CreatorPid == GetCurrentProcessId())
				{
					if (m_Data->FileMappingHandler == NULL || !CloseHandle(m_Data->FileMappingHandler))
					{
						ReleaseMutex(mutex_handler);
						SetLastErrorMessage("HashTable::Close: close file mapping");
						throw GetLastErrorMessage();
					}

					if (m_Data->FileHandler == NULL || !CloseHandle(m_Data->FileHandler))
					{
						ReleaseMutex(mutex_handler);
						SetLastErrorMessage("HashTable::Close: close file handler");
						throw GetLastErrorMessage();
					}

					if (m_Data->TimerHandler != NULL)
					{
						if (!CloseHandle(m_Data->TimerHandler))
						{
							ReleaseMutex(mutex_handler);
							SetLastErrorMessage("HashTable::Close: close timer handler");
							throw GetLastErrorMessage();
						}
					}

					m_Data->FileHandler = NULL;
					m_Data->FileMappingHandler = NULL;
					m_Data->TimerHandler = NULL;
					m_Data->TimerThreadHandler = NULL;
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
				SetLastErrorMessage("HashTable::Close: fail with mutex creation");
				throw GetLastErrorMessage();
			}
		}
		catch (const char* e)
		{
			cout << "\nerror in HashTable::Close: " << e;
			SetLastErrorMessage(e);
			if (mutex_handler != NULL)
			{
				ReleaseMutex(mutex_handler);
			}
			return false;
		}
		catch (...)
		{
			SetLastErrorMessage("unhandled error in HashTable::Close");
			cout << m_Data->LastErrorMessage;
			if (mutex_handler != NULL)
			{
				ReleaseMutex(mutex_handler);
			}
			return false;
		}

		ReleaseMutex(mutex_handler);

		if (!UnmapViewOfFile(m_Data->GetElementsBeginingAddress()))
		{
			SetLastErrorMessage("HashTable::Close: unmapping file error");
			throw GetLastErrorMessage();
		}
		m_Data = NULL;
		return true;
	}


	bool HashTable::Insert(const char* inKey, int inKeyLength, const char* inValue, int inValueLength)
	{
		if (m_Data == nullptr)
		{
			throw "hash table isn't opened, operation is cancelled";
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);

		if (mutex_handler)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inKey);
			if (pointer != NULL)
			{
				ReleaseMutex(mutex_handler);
				SetLastErrorMessage("HashTable::Insert - Hash Table already have such element");
				throw GetLastErrorMessage();
			}
			pointer = GetFreeMemoryForElement();
			if (pointer == NULL)
			{
				ReleaseMutex(mutex_handler);
				SetLastErrorMessage("HashTable::Insert - can't give free memory range for new element");
				throw GetLastErrorMessage();
			}
			else
			{
				Element* new_element = ConvertPointerToElement(pointer);
				string temp_string;
				temp_string = inKey;


				int new_size = min(inKeyLength, m_Data->MaxKeyLength);
				temp_string.resize(new_size);
				new_element->ClearKey();
				new_element->SetKey(temp_string.c_str(), temp_string.length());

				temp_string = inValue;
				new_size = min(inValueLength, m_Data->MaxKeyLength);
				temp_string.resize(new_size);
				new_element->ClearValue();
				new_element->SetValue(temp_string.c_str(), temp_string.length());

				ReleaseMutex(mutex_handler);
				m_Data->CurrentSize++;
			}
		}
		else
		{
			SetLastErrorMessage("HashTable::Insert: fail with mutex creation");
			throw GetLastErrorMessage();
		}

		return true;
	}

	bool HashTable::Delete(const char* inElementKey)
	{
		if (m_Data == nullptr)
		{
			throw "hash table isn't opened, operation is cancelled";
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);
		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inElementKey);
			if (pointer == NULL)
			{
				ReleaseMutex(mutex_handler);
				SetLastErrorMessage("HashTable::Delete - have not found such element");
				throw GetLastErrorMessage();
			}
			else
			{
				ZeroMemory(pointer, sizeof(Element) + m_Data->MaxKeyLength + m_Data->MaxValueLength);
				ReleaseMutex(mutex_handler);
				m_Data->CurrentSize--;
				return true;
			}
		}
		else
		{
			SetLastErrorMessage("HashTable::Delete: fail with mutex creation");
			throw GetLastErrorMessage();
		}
	}

	const Element HashTable::Get(const char* inElementKey)
	{
		if (m_Data == nullptr)
		{
			throw "hash table isn't opened, operation is cancelled";
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inElementKey);
			if (pointer == NULL)
			{
				ReleaseMutex(mutex_handler);
				SetLastErrorMessage("HashTable::Get - have not found such element");
				throw GetLastErrorMessage();
			}
			else
			{
				Element* result = ConvertPointerToElement(pointer);
				ReleaseMutex(mutex_handler);
				return *result;
			}
		}
		else
		{
			SetLastErrorMessage("HashTable::Get: fail with mutex creation");
			throw GetLastErrorMessage();
		}
	}

	bool HashTable::Update(const char* inOldKey, const char* inNewValue, int inNewValueLength)
	{
		if (m_Data == nullptr)
		{
			throw "hash table isn't opened, operation is cancelled";
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inOldKey);
			if (pointer == NULL)
			{
				SetLastErrorMessage("HashTable::Update - there is no such element for updating");
				ReleaseMutex(mutex_handler);
				throw GetLastErrorMessage();
			}
			else
			{
				Element* element = ConvertPointerToElement(pointer);

				string temp_string;
				temp_string = inNewValue;

				int new_size = min(inNewValueLength, m_Data->MaxValueLength);
				temp_string.resize(new_size);
				element->ClearValue();
				element->SetValue(temp_string.c_str(), temp_string.length());

				ReleaseMutex(mutex_handler);
			}
		}
		else
		{
			SetLastErrorMessage("HashTable::Update: fail with mutex creation");
			throw GetLastErrorMessage();
		}

		return true;
	}

	void HashTable::Clear()
	{
		if (m_Data == nullptr)
		{
			throw "hash table isn't opened, operation is cancelled";
		}

		int totalHtSize = GetAllocatedMemorySizeForHashTable(false);
		ZeroMemory(m_Data->GetElementsBeginingAddress(), totalHtSize);
		m_Data->CurrentSize = 0;
	}

	int HashTable::PrintAll()
	{
		if (m_Data == nullptr)
		{
			throw "hash table isn't opened, operation is cancelled";
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer;
			int count_of_elements = 0;
			Element* temp_el;

			while (count_of_elements < m_Data->Capacity)
			{
				pointer = GetMemorySpaceByIndex(count_of_elements);
				temp_el = ConvertPointerToElement(pointer);

				if (temp_el->GetKeyLength() != 0)
				{
					Print(temp_el->GetKey());
					cout << endl;
				}
				count_of_elements++;
			}

			ReleaseMutex(mutex_handler);

			if (count_of_elements == 0)
			{
				cout << "Hash Table is empty\n";
			}

			return count_of_elements;
		}
		else
		{
			SetLastErrorMessage("HashTable::PrintAll: fail with mutex creation");
			throw GetLastErrorMessage();
		}

	}

	void HashTable::Print(const char* inElementKey)
	{
		if (m_Data == nullptr)
		{
			throw "hash table isn't opened, operation is cancelled";
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);

		if (mutex_handler)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			byte* pointer = FindElement(inElementKey);
			if (pointer != NULL)
			{
				Element* result = ConvertPointerToElement(pointer);
				printf("key: %s, value: %s", result->GetKey(), result->GetValue());
			}
			else
			{
				printf("element with key %s is not found", inElementKey);
			}

			ReleaseMutex(mutex_handler);
		}
		else
		{
			SetLastErrorMessage("HashTable::PrintAll: fail with mutex creation");
			throw GetLastErrorMessage();
		}

	}

	bool HashTable::SaveToFile(const char* inFileName)
	{
		if (m_Data == nullptr)
		{
			throw "hash table isn't opened, operation is cancelled";
		}

		HANDLE mutex_handler = CreateMutex(NULL, FALSE, m_Data->MutexName);

		if (mutex_handler != NULL)
		{
			WaitForSingleObject(mutex_handler, INFINITE);

			int totalHtSize = GetAllocatedMemorySizeForHashTable();
			if (FlushViewOfFile(m_Data->GetElementsBeginingAddress(), totalHtSize))
			{
				wstring save_file_path = Utilities::ConvertStringToWstring(Utilities::GetModulePath() + string(inFileName));

				//creating new save file 
				HANDLE file_handler = NULL;
				file_handler = CreateFile(save_file_path.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (file_handler == INVALID_HANDLE_VALUE)
				{
					SetLastErrorMessage("HashTable::SaveToFile: fail with save file creating for hash table");
					ReleaseMutex(mutex_handler);
					throw GetLastErrorMessage();
				}
				else
				{
					//writing memory into created save file 
					DWORD written = -1;
					if (!WriteFile(file_handler, m_Data, totalHtSize, &written, NULL))
					{
						SetLastErrorMessage("HashTable::SaveToFile: fail with writing data to the save file");
						ReleaseMutex(mutex_handler);
						throw GetLastErrorMessage();
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
			SetLastErrorMessage("HashTable::PrintAll: fail with mutex creation");
			throw GetLastErrorMessage();
		}
	}
};
