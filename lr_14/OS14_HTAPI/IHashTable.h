#pragma once
#include <objbase.h>
#include "HashTable.h"
#include <string>

#define FNAME L"KEV.14.COM"
#define VINDX L"KEV.14.1"
#define PRGID L"KEV"

// {A0BBBB8E-72E2-49A1-A8D0-8CD54ECE4512}
static GUID CLSID_CA =
{ 0xa0bbbb8e, 0x72e2, 0x49a1, { 0xa8, 0xd0, 0x8c, 0xd5, 0x4e, 0xce, 0x45, 0x12 } };

// {B56C7F45-59FF-4301-83D7-F6F58A66DE17}
static GUID IID_IHashTable =
{ 0xb56c7f45, 0x59ff, 0x4301, { 0x83, 0xd7, 0xf6, 0xf5, 0x8a, 0x66, 0xde, 0x17 } };


interface IHashTable : IUnknown
{
	STDMETHOD(Create(HT::HashTableData** result, int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName, LPCWSTR HTUsersGroup)) PURE;
	STDMETHOD(CreateFromFile(HT::HashTableData** result, string inFileName)) PURE;

	STDMETHOD(Open(HT::HashTableData** result, string inFileName)) PURE;
	STDMETHOD(Open(HT::HashTableData** result, string inFileName, LPCWSTR userName, LPCWSTR password)) PURE;

	STDMETHOD(Snap(HT::HashTableData** hthandle)) PURE;

	STDMETHOD(Close(HT::HashTableData** hthandle)) PURE;

	STDMETHOD(Insert(HT::HashTableData** hthandle, string inKey, string inValue)) PURE;
	STDMETHOD(Delete(HT::HashTableData** hthandle, string inElementKey)) PURE;
	STDMETHOD(Update(HT::HashTableData** hthandle, string inKey, string inValue)) PURE;
	STDMETHOD(Get(HT::Element* result, HT::HashTableData** hthandle, string inKey)) PURE;

	STDMETHOD(GetLastErrorMessage(std::string& error, HT::HashTableData** ht)) PURE;

};
