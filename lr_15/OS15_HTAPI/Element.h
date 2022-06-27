#pragma once

#define HT_API extern "C" __declspec(dllexport)

namespace HT
{
	struct Element
	{
		char* Key;
		int	KeyLength;
		char* Value;
		int	ValueLength;
	};

	namespace ElementAPI
	{
		HT_API Element* CreateElement();

		HT_API Element* CreateFilledElement(const char* inKey, int inKeyLength, const char* inValue, int inValueLength);

		HT_API void SetKey(Element* inEl, const char* inKey, const int inKeyLength);

		HT_API void SetKeyPointer(Element* inEl, char* inKeyPoint);

		HT_API void ClearKey(Element* inEl);

		HT_API void SetValue(Element* inEl, const char* inValue, const int inValueLength);

		HT_API void SetValuePointer(Element* inEl, char* inValuePoint);

		HT_API void ClearValue(Element* inEl);
	}
}