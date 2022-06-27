#include <objbase.h>
#include <iostream>

// {B852F60C-EA1C-4843-9B96-16C75F9FC1EE}
static const GUID CLSID_OS12 =
{ 0xb852f60c, 0xea1c, 0x4843, { 0x9b, 0x96, 0x16, 0xc7, 0x5f, 0x9f, 0xc1, 0xee } };

// {1347EB8D-0D92-4D69-A1DD-24EC00C6EDCC}
static const GUID IID_IADDER =
{ 0x1347eb8d, 0xd92, 0x4d69, { 0xa1, 0xdd, 0x24, 0xec, 0x0, 0xc6, 0xed, 0xcc } };

interface IADDER : IUnknown
{
	STDMETHOD(Add(const double x, const double y, double& z)) PURE;
	STDMETHOD(Sub(const double x, const double y, double& z)) PURE;
};

// {4D572117-F203-4386-A54F-8A204848E250}
static const GUID IID_IMULTIPLIER =
{ 0x4d572117, 0xf203, 0x4386, { 0xa5, 0x4f, 0x8a, 0x20, 0x48, 0x48, 0xe2, 0x50} };

interface IMULTIPLIER : IUnknown
{
	STDMETHOD(Mul(const double x, const double y, double& z)) PURE;
	STDMETHOD(Div(const double x, const double y, double& z)) PURE;
};


#define IERR(s)    std::cout<<"error "<<s<<std::endl
#define IRES(s,r)  std::cout<<s<<r<<std::endl

IADDER* pIAdder = nullptr;
IMULTIPLIER* pMultiplier = nullptr;


int main()
{
	IUnknown* pIUnknown = NULL;
	CoInitialize(NULL);                        // OLE32
	HRESULT hr0 = CoCreateInstance(CLSID_OS12, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
	if (SUCCEEDED(hr0))
	{
		std::cout << "CoCreateInstance success" << std::endl;
		if (SUCCEEDED(pIUnknown->QueryInterface(IID_IADDER, (void**)&pIAdder)))
		{
			{
				double z = 0.0;
				if (!SUCCEEDED(pIAdder->Add(2.0, 3.0, z)))  
					IERR("IAdder::Add");
				else IRES("IAdder::Add = ", z);
			}
			{
				double z = 0.0;
				if (!SUCCEEDED(pIAdder->Sub(2.0, 3.0, z)))  
					IERR("IAdder::Sub");
				else IRES("IAdder::Sub = ", z);
			}
			if (SUCCEEDED(pIAdder->QueryInterface(IID_IMULTIPLIER, (void**)&pMultiplier)))
			{
				{
					double z = 0.0;
					if (!SUCCEEDED(pMultiplier->Mul(2.0, 3.0, z))) 
						IERR("IMultiplier::Mul");
					else IRES("Multiplier::Mul = ", z);
				}
				{
					double z = 0.0;
					if (!SUCCEEDED(pMultiplier->Div(2.0, 3.0, z))) 
						IERR("IMultiplier::Div");
					else IRES("IMultiplier::Div = ", z);
				}
				if (SUCCEEDED(pMultiplier->QueryInterface(IID_IADDER, (void**)&pIAdder)))
				{
					double z = 0.0;
					if (!SUCCEEDED(pIAdder->Add(2.0, 3.0, z))) 
						IERR("IAdder::Add");
					else IRES("IAdder::Add = ", z);
					pIAdder->Release();
				}
				else  IERR("IMultiplier->IAdder");
				pMultiplier->Release();
			}
			else IERR("IAdder->IMultiplier");
			pIAdder->Release();
		}
		else  IERR("IAdder");
	}
	else  std::cout << "CoCreateInstance error" << std::endl;
	pIUnknown->Release();
	CoFreeUnusedLibraries();                   // завершение работы с библиотекой      

	return 0;
}


