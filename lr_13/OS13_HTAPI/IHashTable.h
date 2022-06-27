#pragma once
#include <objbase.h>
#include "HashTable.h"
#include <string>

#define FNAME L"KEV.COM"
#define VERS L"KEV.1"
#define PRGID L"KEV"

// {55D53EC8-7E0C-4339-892F-CC444C441C9A}
static GUID CLSID_CA =
{ 0x55d53ec8, 0x7e0c, 0x4339, { 0x89, 0x2f, 0xcc, 0x44, 0x4c, 0x44, 0x1c, 0x9a } };

// {14BF8F0E-434C-4ED9-B681-09C367908F92}
static GUID IID_IHashTable =
{ 0x14bf8f0e, 0x434c, 0x4ed9, { 0xb6, 0x81, 0x9, 0xc3, 0x67, 0x90, 0x8f, 0x92 } };


interface IHashTable : IUnknown
{
	STDMETHOD( Create(HT::HashTableData** result, int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName)) PURE;

	STDMETHOD( Open(string inFileName, HT::HashTableData** result)) PURE;

	STDMETHOD( Snap(HT::HashTableData** hthandle)) PURE;

	STDMETHOD( Close(HT::HashTableData** hthandle)) PURE;

	STDMETHOD( Insert(HT::HashTableData** hthandle, string inKey, string inValue)) PURE;
	STDMETHOD( Delete(HT::HashTableData** hthandle, string inElementKey)) PURE;
	STDMETHOD( Update(HT::HashTableData** hthandle, string inKey, string inValue)) PURE;
	STDMETHOD( Get(HT::Element* result, HT::HashTableData** hthandle, string inKey)) PURE;

	STDMETHOD( GetLastErrorMessage(std::string& error, HT::HashTableData** ht)) PURE;

};

