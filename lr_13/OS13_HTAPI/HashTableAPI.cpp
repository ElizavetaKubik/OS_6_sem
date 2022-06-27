#include <Windows.h>
#include <objbase.h>
#include "HashTableAPI.h"
#include "IHashTable.h"
#include <iostream>

namespace HTAPI
{
	IHashTable* pIHashTable = nullptr;
	IUnknown* pIUnknown = NULL;

	bool  HTAPI::OpenApi()
	{
		CoInitialize(NULL);                        // OLE32
		HRESULT hr0 = CoCreateInstance(CLSID_CA, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
		if (SUCCEEDED(hr0))
		{
			std::cout << "CoCreateInstance succeeded" << std::endl;
			if (SUCCEEDED(pIUnknown->QueryInterface(IID_IHashTable, (void**)&pIHashTable)))
			{
				return true;
			}
		}
		return false;
	}

	bool  HTAPI::CloseApi()
	{
		bool res = true;
		if (!SUCCEEDED(pIHashTable->Release()))
			res = false;
		if (!SUCCEEDED(pIUnknown->Release()))
			res = false;
		CoFreeUnusedLibraries();
		return true;
	}


	HT::HashTableData* HTAPI::Create(int inCapacity, int   inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName)
	{
		HT::HashTableData* pht;
		if (SUCCEEDED(pIHashTable->Create(&pht, inCapacity, inSecSnapshotInterval, inMaxKeyLength, inMaxValueLength, inFileName)))
		{
			return pht;
		}
		else
		{
			return NULL;
		}
	}

	HT::HashTableData* HTAPI::Open(string inFileName)
	{
		HT::HashTableData* pht;
		if (SUCCEEDED(pIHashTable->Open(inFileName, &pht)))
		{
			return pht;
		}
		else
		{
			return NULL;
		}
	}

	BOOL Snap(HT::HashTableData* hthandle)
	{
		if (SUCCEEDED(pIHashTable->Snap(&hthandle)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	BOOL Close(HT::HashTableData* hthandle)
	{
		if (SUCCEEDED(pIHashTable->Close(&hthandle)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	BOOL Insert(HT::HashTableData* hthandle, string inKey, string inValue)
	{
		if (SUCCEEDED(pIHashTable->Insert(&hthandle, inKey, inValue)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	BOOL Delete(HT::HashTableData* hthandle, string inKey)
	{
		if (SUCCEEDED(pIHashTable->Delete(&hthandle, inKey)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	HT::Element* Get(HT::HashTableData* hthandle, string inKey)
	{
		HT::Element* result = NULL;
		if (SUCCEEDED(pIHashTable->Get(result, &hthandle, inKey)))
		{
			return result;
		}
		else
		{
			return NULL;
		}
	}

	BOOL Update(HT::HashTableData* hthandle, string inKey, string inValue)
	{
		if (SUCCEEDED(pIHashTable->Update(&hthandle, inKey, inValue)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	char* GetLastHtError(HT::HashTableData* ht)
	{
		std::string err;
		if (SUCCEEDED(pIHashTable->GetLastErrorMessage(err, &ht)))
		{
			return (char*)(err.c_str());
		}
		else
		{
			return NULL;
		}
	}

};
