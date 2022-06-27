#pragma once
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

namespace OS12_LIB {
	void* Init();

	namespace ADDER {
		double Add(void* h, double x, double y);
		double Sub(void* h, double x, double y);
	}

	namespace MULTIPLIER {
		double Mul(void* h, double x, double y);
		double Div(void* h, double x, double y);
	}
	void Dispose(void* h);
}