#pragma once
#include <objbase.h>
#include "HashTable.h"
#include <string>

#define FNAME L"KEV.15.COM"
#define VINDX L"KEV.15.1"
#define PRGID L"KEV"

// {72B636D8-3D60-4FB8-8425-16E02D2718F3}
static GUID CLSID_CA =
{ 0x72b636d8, 0x3d60, 0x4fb8, { 0x84, 0x25, 0x16, 0xe0, 0x2d, 0x27, 0x18, 0xf3 } };

// {9A6F6403-28BC-4CCE-8DB1-7001A7BF6CFB}
static GUID IID_IHashTable =
{ 0x9a6f6403, 0x28bc, 0x4cce, { 0x8d, 0xb1, 0x70, 0x1, 0xa7, 0xbf, 0x6c, 0xfb } };


interface IHashTable : IUnknown
{
	STDMETHOD( Create(HT::HashTableData** result, int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName, LPCWSTR HTUsersGroup)) PURE;
	STDMETHOD( CreateFromFile(HT::HashTableData** result, string inFileName)) PURE;

	STDMETHOD( Open(HT::HashTableData** result, string inFileName)) PURE;
	STDMETHOD( Open(HT::HashTableData** result, string inFileName, LPCWSTR userName, LPCWSTR password)) PURE;

	STDMETHOD( Snap(HT::HashTableData** hthandle)) PURE;

	STDMETHOD( Close(HT::HashTableData** hthandle)) PURE;

	STDMETHOD( Insert(HT::HashTableData** hthandle, string inKey, string inValue)) PURE;
	STDMETHOD( Delete(HT::HashTableData** hthandle, string inElementKey)) PURE;
	STDMETHOD( Update(HT::HashTableData** hthandle, string inKey, string inValue)) PURE;
	STDMETHOD( Get(HT::Element* result, HT::HashTableData** hthandle, string inKey)) PURE;

	STDMETHOD( GetLastErrorMessage(std::string& error, HT::HashTableData** ht)) PURE;

};
