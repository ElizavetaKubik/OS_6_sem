#include "Element.h"
#include <Windows.h>

namespace HT
{
	HT_API Element* ElementAPI::CreateElement()
	{
		Element* el = new Element();
		el->KeyLength = 0;
		el->Key = nullptr;

		el->ValueLength = 0;
		el->Value = nullptr;

		return el;
	}

	HT_API Element* ElementAPI::CreateFilledElement(const char* inKey, int inKeyLength, const char* inValue, int inValueLength)
	{
		Element* el = new Element();
		ElementAPI::SetKey(el, inKey, inKeyLength);
		ElementAPI::SetValue(el, inValue, inValueLength);

		return el;
	}

	void ElementAPI::SetKey(Element* inEl, const char* inKey, const int inKeyLength)
	{
		if (inEl == nullptr)
			return;

		for (int i = 0; i < inKeyLength; i++)
		{
			inEl->Key[i] = inKey[i];
		}

		inEl->KeyLength = inKeyLength;
	}

	void ElementAPI::SetKeyPointer(Element* inEl, char* inKeyPoint)
	{
		if (inEl == nullptr)
			return;

		inEl->Key = inKeyPoint;
	}

	void ElementAPI::ClearKey(Element* inEl)
	{
		if (inEl == nullptr)
			return;

		ZeroMemory(inEl->Key, inEl->KeyLength);
		inEl->KeyLength = 0;
	}

	void ElementAPI::SetValue(Element* inEl, const char* inValue, const int inValueLength)
	{
		if (inEl == nullptr)
			return;

		for (int i = 0; i < inValueLength; i++)
		{
			inEl->Value[i] = inValue[i];
		}

		inEl->ValueLength = inValueLength;
	}

	void ElementAPI::SetValuePointer(Element* inEl, char* inValuePoint)
	{
		if (inEl == nullptr)
			return;

		inEl->Value = inValuePoint;
	}

	void ElementAPI::ClearValue(Element* inEl)
	{
		if (inEl == nullptr)
			return;

		ZeroMemory(inEl->Value, inEl->ValueLength);
		inEl->ValueLength = 0;
	}
}

