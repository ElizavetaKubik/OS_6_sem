#include <windows.h>
#include <objbase.h>
#include "Interface.h"
#include "ClassFactory.h"
#include "SEQLOG.h"

ULONG g_Components;
ULONG g_ServerLocks;

STDAPI DllCanUnloadNow() {

	SEQ;
	HRESULT rc = E_UNEXPECTED;
	if ((g_ServerLocks == 0) && (g_Components == 0))
		rc = S_OK;
	else rc = S_FALSE;
	LOG("DllCanUnloadNow rc = ", rc);
	return rc;
}
STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv) {
	SEQ;
	HRESULT rc = E_UNEXPECTED;
	ClassFactory* pF;
	if (clsid != CLSID_OS12) 
		rc = CLASS_E_CLASSNOTAVAILABLE;
	else if ((pF = new ClassFactory()) == NULL)
		rc = E_OUTOFMEMORY;
	else {
		rc = pF->QueryInterface(iid, ppv);
		pF->Release();
	}
	LOG("DllGetClassObject: rc = ", rc);
	return rc;
}