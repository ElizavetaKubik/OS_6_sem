#pragma once
#include "ClassFactory.h"
#include "SEQLOG.h"
#include "HT_COM.h"

extern ULONG g_ServerLocks;

ClassFactory::ClassFactory() :m_Ref(1) 
{ 
	SEQ; LOG("CFactory::CFactory", ""); 
};

ClassFactory::~ClassFactory() 
{ 
	SEQ; LOG("CFactory::~CFactory", ""); 
};

HRESULT STDMETHODCALLTYPE ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	SEQ;
	HRESULT rc = S_OK;
	*ppv = NULL;
	if (riid == IID_IUnknown)	
		*ppv = (IUnknown*)this;
	else if (riid == IID_IClassFactory)	
		*ppv = (IClassFactory*)this;
	else 
		rc = E_NOINTERFACE;

	if (rc == S_OK) 
		this->AddRef();
	LOG("Cfactory::QueryInterface rc = ", rc);
	return rc;
};

ULONG STDMETHODCALLTYPE ClassFactory::AddRef(void)
{
	SEQ;
	InterlockedIncrement((LONG*)&(this->m_Ref));
	LOG("CFactory::AddRef m_Ref = ", this->m_Ref);
	return this->m_Ref;
};

ULONG STDMETHODCALLTYPE ClassFactory::Release(void)
{
	SEQ;

	ULONG rc = this->m_Ref;
	if ((rc = InterlockedDecrement((LONG*)&(this->m_Ref))) == 0) 
		delete this;
	LOG("CFactory::Release rc = ", rc);
	return rc;
};


HRESULT STDMETHODCALLTYPE ClassFactory::CreateInstance(IUnknown* pUO, const IID& id, void** ppv)
{
	SEQ;
	HRESULT rc = E_UNEXPECTED;

	HT_COM* ht_com;

	if (pUO != NULL)	
		rc = CLASS_E_NOAGGREGATION;
	else if ((ht_com = new HT_COM()) == NULL)	
		rc = E_OUTOFMEMORY;
	else
	{
		rc = ht_com->QueryInterface(id, ppv);
		ht_com->Release();
	}

	LOG("CFactory::CreateInstance rc = ", rc);
	return rc;
}

HRESULT STDMETHODCALLTYPE ClassFactory::LockServer(BOOL b)
{
	SEQ;
	HRESULT rc = S_OK;

	if (b) 
		InterlockedIncrement((LONG*)&(g_ServerLocks));
	else 
		InterlockedDecrement((LONG*)&(g_ServerLocks));

	LOG("CFactory::LockServer b = ", b);
	return rc;
}
