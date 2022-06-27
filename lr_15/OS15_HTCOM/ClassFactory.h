#pragma once
#include <objbase.h>

class ClassFactory : public IClassFactory
{
public:
	ClassFactory();
	~ClassFactory();

	STDMETHOD(QueryInterface(REFIID riid, void** ppv));
	STDMETHOD_(ULONG, AddRef(void));
	STDMETHOD_(ULONG, Release(void));


	STDMETHOD(CreateInstance(IUnknown* pUO, const IID& id, void** ppv));
	STDMETHOD(LockServer(BOOL b));

private:
	ULONG m_Ref;
};