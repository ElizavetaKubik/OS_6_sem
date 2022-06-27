#pragma once

namespace HT
{
	class Element
	{
	public:

		Element();

		Element(const char* inKey, int inKeyLength, const char* inValue, int  inValueuLength);

		const char* GetKey() const;

		void SetKey(const char* inKey, const int inKeyLength);

		void SetKeyPointer(char* inKeyPoint);

		void ClearKey();

		const int GetKeyLength() const;

		const char* GetValue() const;

		void SetValue(const char* inValue, const int inValueLength);

		void SetValuePointer(char* inValuePoint);

		void ClearValue();

		const int GetValueLength() const;

	private:

		char* m_Key;
		int	m_KeyLength;
		char* m_Value;
		int	m_ValueLength;
	};
}