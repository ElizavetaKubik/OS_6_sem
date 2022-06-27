#include <windows.h>
#include "OS12.h"
#include "SEQLOG.h"

extern ULONG g_Components;

OS12::OS12() : m_Ref(1)
{
	SEQ;
	InterlockedIncrement((LONG*)&g_Components);
	LOG("OS12::OS12 g_Components = ", g_Components);
};

OS12::~OS12()
{
	SEQ;
	InterlockedDecrement((LONG*)&g_Components);
	LOG("OS12::~OS12 g_Components = ", g_Components);
}

STDMETHODIMP OS12::QueryInterface(REFIID riid, void** ppv)
{
	SEQ;
	HRESULT rc = S_OK;
	*ppv = NULL;
	if (riid == IID_IUnknown) 
		*ppv = static_cast<IADDER*>(this);
	else if (riid == IID_IADDER) 
		*ppv = static_cast<IADDER*>(this);
	else if (riid == IID_IMULTIPLIER) 
		*ppv = static_cast<IMULTIPLIER*>(this);
	else rc = E_NOINTERFACE;
	if (rc == S_OK) this->AddRef();
	LOG("OS12::QueryInterface rc = ", rc);
	return rc;
};

STDMETHODIMP_(ULONG) OS12::AddRef(void)
{
	SEQ;
	InterlockedIncrement((LONG*)&(this->m_Ref));
	LOG("OS12::AddRef rc = ", this->m_Ref);
	return this->m_Ref;
};

STDMETHODIMP_(ULONG) OS12::Release(void)
{
	SEQ;
	ULONG  rc = this->m_Ref;
	if ((rc = InterlockedDecrement((LONG*)&(this->m_Ref))) == 0) delete this;
	LOG("OS12::Release rc = ", this->m_Ref);
	return this->m_Ref;
};

STDMETHODIMP OS12::Add(const double x, const double y, double& z)
{
	SEQ;
	z = x + y;
	LOG("OS12:Add rc = ", S_OK);
	return S_OK;
};

STDMETHODIMP OS12::Sub(const double x, const double y, double& z)
{
	SEQ;
	z = x - y;
	LOG("OS12:Sub rc = ", S_OK);
	return S_OK;
};

STDMETHODIMP OS12::Mul(const double x, const double y, double& z)
{
	SEQ;
	z = x * y;
	LOG("OS12:Mul rc = ", S_OK);
	return S_OK;
};

STDMETHODIMP OS12::Div(const double x, const double y, double& z)
{
	SEQ;
	HRESULT rc = S_OK;
	if (y != 0)
		z = x / y;
	else
		rc = S_FALSE;
	LOG("OS12:Div rc = ", rc);
	return rc;
};
