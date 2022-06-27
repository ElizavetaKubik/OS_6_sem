#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <string>
#include "../OS14_HTAPI/Utilities.h"
#include "../OS14_HTAPI/HashTable.h"
#include "../OS14_HTAPI/HashTableAPI.h"

#pragma comment(lib,"../Debug/OS14_HTAPI.lib")

using namespace std;

#define SEC 1000
#define VALUE_ARRAY_SIZE 3

char randomSymbol();
void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* inValue, bool inSuccess);

int main(int argc, char* argv[])
{
	srand(time(0));

	string key_prefix = "key_";
	wstring log_file_name = Utilities::ConvertStringToWstring(Utilities::GetModulePath() + "insert.log");

	if (argc == 2)
	{
		try
		{
			HTAPI::OpenApi();

			string file_name = argv[1];

			HT::HashTableData* ht = HTAPI::Open(file_name, L"HTUser01", L"01111");

			HANDLE log_file_handler = CreateFile(log_file_name.c_str(), GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (log_file_handler == INVALID_HANDLE_VALUE)
			{
				std::cout << "can't open log file: " << log_file_name.c_str() << std::endl;
				system("pause");
				return 0;
			}

			string insert_key;
			char insert_value[VALUE_ARRAY_SIZE]{};
			bool success_result;
			while (true)
			{
				insert_key = key_prefix + randomSymbol();
				_itoa(rand() % 100, insert_value, 10);

				std::cout << "Inserting: " << insert_key << std::endl;

				try
				{
					success_result = HTAPI::Insert(ht, insert_key, insert_value);
				}
				catch (const char* exeptionMessage)
				{
					cout << "exception: " << exeptionMessage << endl;
					success_result = false;
				}
				catch (...)
				{
					cout << "unhandled exeption" << endl;
					success_result = false;
				}

				WriteRowInLog(log_file_handler, insert_key.c_str(), insert_value, success_result);

				Sleep(SEC);
			}

			CloseHandle(log_file_handler);

			HTAPI::Close(ht);

		}
		catch (string exeptionMessage)
		{
			cout << "exception: " << exeptionMessage << endl;
		}
		catch (...)
		{
			cout << "unhandled exeption" << endl;
		}
	}
	else
	{
		cout << "Error in params\n";
	}

	system("pause");
	return 0;
}

char randomSymbol()
{
	int minValue = 'a';
	int delta = 50;
	return  minValue + rand() % delta;
}

void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* inValue, bool inSuccess)
{
	string message("");
	char time_buffer[80]{ 0 };

	time_t seconds = time(NULL);
	tm* time_info = localtime(&seconds);
	const char* format = "%e.%m.%Y %I:%M:%S";
	strftime(time_buffer, 80, format, time_info);

	message += time_buffer;
	message += " Insert: key = ";
	message += inKey;
	message += " value = ";
	message += inValue;
	inSuccess ? message += " TRUE\n" : message += " FALSE\n";

	LARGE_INTEGER file_pointer;
	file_pointer.QuadPart = 0;
	DWORD positionMode = FILE_END;

	if (!SetFilePointerEx(inLogFileHandler, file_pointer, NULL, positionMode))
	{
		throw "SetFilePointerEx return False";
	}

	if (!SetEndOfFile(inLogFileHandler))
	{
		throw "SetEndOfFile return False";
	}

	DWORD out_length = NULL;
	WriteFile(inLogFileHandler, message.c_str(), message.length(), &out_length, NULL);
}