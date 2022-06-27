#include "HT_COM.h"
#include "SEQLOG.h"
#include "HashTable.h"
#include "Utilities.h"
#include <string>

extern ULONG g_Components;

HT_COM::HT_COM() :m_Ref(1)
{
	SEQ;
	InterlockedIncrement((LONG*)&g_Components);
	LOG("HT_COM::Adder g_Components = ", g_Components);
};

HT_COM::~HT_COM()
{
	SEQ;
	InterlockedDecrement((LONG*)&g_Components);
	LOG("HT_COM::~Adder g_Components = ", g_Components);
};


STDMETHODIMP HT_COM::QueryInterface(REFIID riid, void** ppv)
{
	SEQ;
	HRESULT rc = S_OK;
	*ppv = NULL;
	if (riid == IID_IUnknown)
		*ppv = (IHashTable*)this;
	else if (riid == IID_IHashTable)
		*ppv = (IHashTable*)this;
	else rc = E_NOINTERFACE;

	if (rc == S_OK)
		this->AddRef();
	LOG("HT_COM::QueryInterface rc = ", rc);
	return rc;
};

STDMETHODIMP_(ULONG) HT_COM::AddRef(void)
{
	SEQ;
	InterlockedIncrement((LONG*)&(this->m_Ref));
	LOG("HT_COM::AddRef m_Ref = ", this->m_Ref);
	return this->m_Ref;
};

STDMETHODIMP_(ULONG) HT_COM::Release(void)
{
	SEQ;

	ULONG rc = this->m_Ref;
	if ((rc = InterlockedDecrement((LONG*)&(this->m_Ref))) == 0) 
		delete this;
	LOG("HT_COM::Release rc = ", rc);
	return rc;
};


STDMETHODIMP HT_COM::Create(HT::HashTableData** result, int inCapacity, int inSecSnapshotInterval, int inMaxKeyLength, int inMaxValueLength, string inFileName, LPCWSTR HTUsersGroup)
{
	*result = HT::CreateHT(inCapacity, inSecSnapshotInterval, inMaxKeyLength, inMaxValueLength, inFileName, HTUsersGroup);

	SEQHT;
	LOGHT("Created ht result: ", &result);
	return S_OK;
}

STDMETHODIMP HT_COM::CreateFromFile(HT::HashTableData** result, string inFileName)
{
	*result = HT::CreateHT_FromFile(inFileName.c_str(), L"HT");

	SEQHT;
	LOGHT("Created ht result: ", &result);
	return S_OK;
}

STDMETHODIMP HT_COM::Open(HT::HashTableData** result, string inFileName)
{
	SEQHT;
	LOGHT("Open file with name: ", inFileName);

	HT::HashTableData* temp = HT::Open(Utilities::GetModulePath() + inFileName);
	if (temp == NULL)
	{
		return E_FAIL;
	}

	*result = temp;

	if (*result != NULL)
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP HT_COM::Open(HT::HashTableData** result, string inFileName, LPCWSTR userName, LPCWSTR password)
{
	SEQHT;
	LOGHT("Open file with name: ", inFileName);

	HT::HashTableData* temp = HT::Open(Utilities::GetModulePath() + inFileName, userName, password);
	if (temp == NULL)
	{
		return E_FAIL;
	}

	*result = temp;

	if (*result != NULL)
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP  HT_COM::Snap(HT::HashTableData** hthandle)
{
	if (HT::Snap(*hthandle))
		return S_OK;
	else
		return E_FAIL;
}


STDMETHODIMP HT_COM::Close(HT::HashTableData** hthandle)
{
	bool res = HT::Close(*hthandle);

	if (res)
		return S_OK;
	else
		return E_FAIL;
}


STDMETHODIMP HT_COM::Insert(HT::HashTableData** hthandle, string inKey, string inValue)
{
	if (HT::Insert(*hthandle, inKey.c_str(), inKey.length(), inValue.c_str(), inValue.length()))
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP HT_COM::Delete(HT::HashTableData** hthandle, string inElementKey)
{
	if (HT::Delete(*hthandle, inElementKey.c_str()))
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP HT_COM::Get(HT::Element* result, HT::HashTableData** hthandle, string inKey)
{
	SEQHT;
	HT::Element* temp = HT::Get(*hthandle, inKey.c_str());
	LOGHT("Get element with temp: ", temp);
	if (temp == NULL)
		return E_FAIL;
	result = temp;

	return S_OK;
}

STDMETHODIMP  HT_COM::Update(HT::HashTableData** hthandle, string inKey, string inValue)
{
	if (HT::Update(*hthandle, inKey.c_str(), inValue.c_str(), inValue.length()))
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP HT_COM::GetLastErrorMessage(std::string& error, HT::HashTableData** ht)
{
	error = std::string(HT::GetLastErrorMessage(*ht));
	LOGHT("Ht error:  ", error);
	return S_OK;
}