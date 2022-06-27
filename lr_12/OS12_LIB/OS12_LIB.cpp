#include "OS12_LIB.h"

namespace OS12_LIB {
	void* Init() {
		IUnknown* pIUnknown = NULL;
		CoInitialize(NULL);
		HRESULT hr = CoCreateInstance(CLSID_OS12, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
		if (!SUCCEEDED(hr)) throw (int)hr;
		return pIUnknown;
	}

	void Dispose(void* h) {
		IUnknown* pIUnknown = static_cast<IUnknown*>(h);
		pIUnknown->Release();
		CoFreeUnusedLibraries();
	}

	namespace ADDER {
		double Add(void* h, double x, double y) {
			IUnknown* pIUnknown = static_cast<IUnknown*>(h);
			IADDER* pIAdder = nullptr;
			HRESULT hr = pIUnknown->QueryInterface(IID_IADDER, (void**)&pIAdder);
			if (!SUCCEEDED(hr)) 
				throw (int)hr;
			double z = 0.0;

			hr = pIAdder->Add(x, y, z);
			pIAdder->Release();
			if (!SUCCEEDED(hr)) 
				throw (int)hr;
			else return z;
		}
		double Sub(void* h, double x, double y) {
			IUnknown* pIUnknown = static_cast<IUnknown*>(h);
			IADDER* pIAdder = nullptr;
			HRESULT hr = pIUnknown->QueryInterface(IID_IADDER, (void**)&pIAdder);
			if (!SUCCEEDED(hr)) 
				throw (int)hr;
			double z = 0.0;

			hr = pIAdder->Sub(x, y, z);
			pIAdder->Release();
			if (!SUCCEEDED(hr)) 
				throw (int)hr;
			else return z;
		}
	}
	namespace MULTIPLIER {
		double Mul(void* h, double x, double y) {
			IUnknown* pIUnknown = static_cast<IUnknown*>(h);
			IMULTIPLIER* pIMultiplier = nullptr;
			HRESULT hr = pIUnknown->QueryInterface(IID_IMULTIPLIER, (void**)&pIMultiplier);
			if (!SUCCEEDED(hr)) 
				throw (int)hr;
			double z = 0.0;

			hr = pIMultiplier->Mul(x, y, z);
			pIMultiplier->Release();

			if (!SUCCEEDED(hr)) 
				throw (int)hr;
			else return z;
		}
		double Div(void* h, double x, double y) {
			if (y == 0) throw "DivideBySeroException";
			IUnknown* pIUnknown = static_cast<IUnknown*>(h);
			IMULTIPLIER* pIMultiplier = nullptr;
			HRESULT hr = pIUnknown->QueryInterface(IID_IMULTIPLIER, (void**)&pIMultiplier);
			if (!SUCCEEDED(hr)) 
				throw (int)hr;
			double z = 0.0;
			hr = pIMultiplier->Mul(x, y, z);
			pIMultiplier->Release();
			if (!SUCCEEDED(hr)) 
				throw (int)hr;
			else return z;
		}
	}
}