#pragma once
#include <Windows.h>
#include "HashTable.h"
#include "Utilities.h"

namespace HTAPI
{
	bool OpenApi();
	bool CloseApi();

	HT::HashTableData* Create(int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName, LPCWSTR HTUsersGroup);
	HT::HashTableData* CreateFromFile(string inFileName);

	HT::HashTableData* Open(string inFileName);

	HT::HashTableData* Open(string inFileName, LPCWSTR userName, LPCWSTR password);

	BOOL Snap(HT::HashTableData* hthandle);

	BOOL Close(HT::HashTableData* hthandle);

	BOOL Insert(HT::HashTableData* hthandle, string inKey, string inValue);
	BOOL Delete(HT::HashTableData* hthandle, string inKey);
	BOOL Update(HT::HashTableData* hthandle, string inKey, string inValue);

	HT::Element* Get(HT::HashTableData* hthandle, string inKey);

	char* GetLastHtError(HT::HashTableData* ht);

};
