#include <windows.h>
#include "ClassFactory.h"
#include "OS12.h"
#include "SEQLOG.h"

ClassFactory::ClassFactory() :m_Ref(1) {
	SEQ;
	LOG("CFactory::CFactory()", "");
};

ClassFactory::~ClassFactory() {
	SEQ;
	LOG("CFactory::~CFactory()", "");
};

STDMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	SEQ;
	HRESULT rc = S_OK;
	*ppv = NULL;
	if (riid == IID_IUnknown) 
		*ppv = static_cast<IUnknown*>(this);
	else if (riid == IID_IClassFactory) 
		*ppv = static_cast<IClassFactory*>(this);
	else rc = E_NOINTERFACE;

	if (rc == S_OK) this->AddRef();
	LOG("CFactory::QueryInterface rc = ", rc);
	return rc;
};

STDMETHODIMP_(ULONG) ClassFactory::AddRef(void)
{
	SEQ;
	InterlockedIncrement((LONG*)&(this->m_Ref));
	LOG("CFactory::AddRef rc = ", this->m_Ref);
	return this->m_Ref;
};

STDMETHODIMP_(ULONG) ClassFactory::Release(void)
{
	SEQ;
	ULONG  rc = this->m_Ref;
	if ((rc = InterlockedDecrement((LONG*)&(this->m_Ref))) == 0) 
		delete this;
	LOG("CFactory::Release rc = ", this->m_Ref);
	return this->m_Ref;
};

STDMETHODIMP ClassFactory::LockServer(BOOL b) //TODO: debug
{
	SEQ;
	HRESULT rc = S_OK;
	LOG("CFactory::LockServer rc = ", rc);
	return rc;
}

STDMETHODIMP ClassFactory::CreateInstance(IUnknown* pUO, const IID& id, void** ppv)
{
	SEQ;
	HRESULT rc = E_UNEXPECTED;
	OS12* pOS12;
	if (pUO != NULL) 
		rc = CLASS_E_NOAGGREGATION;
	else if ((pOS12 = new OS12()) == NULL) 
		rc = E_OUTOFMEMORY;
	else {
		rc = pOS12->QueryInterface(id, ppv);
		pOS12->Release();
	}
	LOG("CFactory::CreateInstance rc = ", rc);
	return rc;
}