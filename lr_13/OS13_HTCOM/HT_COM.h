#pragma once
#include <objbase.h>
#include <iostream>
#include "IHashTable.h"
#include <string>

class HT_COM : public IHashTable
{
public:
	HT_COM();
	~HT_COM();

	STDMETHOD( QueryInterface(REFIID riid, void** ppv));
	STDMETHOD_( ULONG, AddRef(void));
	STDMETHOD_( ULONG, Release(void));

	STDMETHOD( Create(HT::HashTableData** result, int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName));
	
	STDMETHOD( Open(string inFileName, HT::HashTableData** result));
	STDMETHOD( Snap(HT::HashTableData** hthandle));

	STDMETHOD( Close(HT::HashTableData** hthandle));
	STDMETHOD( Insert(HT::HashTableData** hthandle, string inKey, string inValue));
	STDMETHOD( Update(HT::HashTableData** hthandle, string inKey, string inValue));
	STDMETHOD( Delete(HT::HashTableData** hthandle, string inElementKey));
	STDMETHOD( Get(HT::Element* result, HT::HashTableData** hthandle, string inKey));

	STDMETHOD( GetLastErrorMessage(std::string& error, HT::HashTableData** ht));

private:
	volatile ULONG m_Ref;
};
