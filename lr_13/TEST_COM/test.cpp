#include <objbase.h>
#include <iostream>
#include "HashTable.h"

// {55D53EC8-7E0C-4339-892F-CC444C441C9A}
static const GUID CLSID_CA =
{ 0x55d53ec8, 0x7e0c, 0x4339, { 0x89, 0x2f, 0xcc, 0x44, 0x4c, 0x44, 0x1c, 0x9a } };

// {14BF8F0E-434C-4ED9-B681-09C367908F92}
static const GUID IID_IHashTable =
{ 0x14bf8f0e, 0x434c, 0x4ed9, { 0xb6, 0x81, 0x9, 0xc3, 0x67, 0x90, 0x8f, 0x92 } };

interface IHashTable : IUnknown
{
	STDMETHOD(Create(HT::HashTableData** result, int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName)) PURE;

	STDMETHOD(Open(string inFileName, HT::HashTableData** result)) PURE;

	STDMETHOD(Snap(HT::HashTableData** hthandle)) PURE;

	STDMETHOD(Close(HT::HashTableData** hthandle)) PURE;

	STDMETHOD(Insert(HT::HashTableData** hthandle, string inKey, string inValue)) PURE;
	STDMETHOD(Delete(HT::HashTableData** hthandle, string inElementKey)) PURE;
	STDMETHOD(Update(HT::HashTableData** hthandle, string inKey, string inValue)) PURE;
	STDMETHOD(Get(HT::Element& result, HT::HashTableData** hthandle, string inKey)) PURE;

	STDMETHOD(GetLastErrorMessage(std::string& error, HT::HashTableData** ht)) PURE;

};


#define IERR(s)    std::cout<<"error "<<s<<std::endl
#define IRES(s,r)  std::cout<<s<<r<<std::endl

IHashTable* pHashTable = nullptr;


int main()
{
	IUnknown* pIUnknown = NULL;
	CoInitialize(NULL);                        // OLE32
	HRESULT hr0 = CoCreateInstance(CLSID_CA, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
	if (SUCCEEDED(hr0))
	{
		std::cout << "CoCreateInstance success" << std::endl;
		if (SUCCEEDED(pIUnknown->QueryInterface(IID_IHashTable, (void**)&pHashTable)))
		{
			{
				//double z = 0.0;
				//if (!SUCCEEDED(pHashTable->Sub(2.0, 3.0, z)))
				//	IERR("IAdder::Sub");
				//else IRES("IAdder::Sub = ", z);
			}
			pHashTable->Release();
		}
		else  IERR("pHashTable");
	}
	else  
		std::cout << "CoCreateInstance error" << std::endl;
	pIUnknown->Release();
	CoFreeUnusedLibraries();                   // завершение работы с библиотекой      

	return 0;
}


