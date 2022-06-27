#pragma once
#include <objbase.h>
#include <iostream>
#include "Interface.h"

class OS12 : public IADDER, public IMULTIPLIER
{
public:
	OS12();
	~OS12();

	STDMETHOD( QueryInterface(REFIID riid, void** ppv));
	STDMETHOD_( ULONG, AddRef(void));
	STDMETHOD_( ULONG, Release(void));
	
	STDMETHOD( Add(const double x, const double y, double& z));
	STDMETHOD( Sub(const double x, const double y, double& z));
	STDMETHOD( Mul(const double x, const double y, double& z));
	STDMETHOD( Div(const double x, const double y, double& z));

private:
	volatile ULONG m_Ref;
};