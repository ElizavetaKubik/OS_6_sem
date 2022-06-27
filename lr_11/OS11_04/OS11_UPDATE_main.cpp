#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <string>
#include "../HTAPI/Utilities.h"
#include "../HTAPI/HashTable.h"

#pragma comment(lib,"../Debug/HTAPI.lib")

using namespace std;

#define SEC 1000
#define VALUE_ARRAY_SIZE 3

char randomSymbol();
void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* inOldValue, const char* inNewValue, bool inSuccess);

typedef bool(*Update)(HT::HashTableData*, const char*, const char*, int);


int main(int argc, char* argv[])
{
	srand(time(0));

	string key_prefix = "key_";
	wstring log_file_name = Utilities::ConvertStringToWstring(Utilities::GetModulePath() + "update.log");
	if (argc == 2)
	{
		try
		{
			HINSTANCE dll = LoadLibrary(TEXT("HTAPI.dll"));

			string file_path = Utilities::GetModulePath() + argv[1];

			HT::HashTableData* hash_table_data = HT::OpenHT_FromFile(file_path);


			if (dll == NULL) {
				std::cout << "can't open dll\n";
				system("pause");
				return 0;
			}

			Update updateF = (Update)GetProcAddress((HMODULE)dll, "Update");

			if (!updateF)
			{
				std::cout << "can't get this function\n";

				FreeLibrary(dll);

				system("pause");
				return 0;
			}

			HANDLE log_file_handler = CreateFile(log_file_name.c_str(), GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (log_file_handler == INVALID_HANDLE_VALUE)
			{
				std::cout << "can't open log file: " << log_file_name.c_str() << std::endl;
				system("pause");
				return 0;
			}

			string temp_key;
			char temp_old_value[VALUE_ARRAY_SIZE]{ 0 };
			char temp_new_value[VALUE_ARRAY_SIZE]{ 0 };
			bool success_result;
			HT::Element element = HT::Element();
			while (true)
			{
				temp_key = key_prefix + randomSymbol();

				std::cout << "Updating: " << temp_key << std::endl;

				try
				{
					element = HT::Get(hash_table_data, temp_key.c_str());
					if (element.KeyLength>0) {
						_itoa(atoi(element.Value), temp_old_value, 10);
						_itoa(atoi(element.Value) + 1, temp_new_value, 10);

						success_result = HT::Update(hash_table_data, temp_key.c_str(), temp_new_value, strlen(temp_new_value));
					}
					else {
						cout << "can't find this element\n";
						success_result = false;
					}

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


				WriteRowInLog(log_file_handler, temp_key.c_str(), temp_old_value, temp_new_value, success_result);

				Sleep(SEC);
			}

			CloseHandle(log_file_handler);

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

void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* inOldValue, const char* inNewValue, bool inSuccess)
{
	string message;
	char time_buffer[80]{ 0 };

	time_t seconds = time(NULL);
	tm* time_info = localtime(&seconds);
	const char* format = "%e.%m.%Y %I:%M:%S";
	strftime(time_buffer, 80, format, time_info);

	message += time_buffer;
	message += " Update: key = ";
	message += inKey;

	if (inSuccess)
	{
		message += " old vlaue = ";
		message += inOldValue;
		message += " new vlaue = ";
		message += inNewValue;
		message += " TRUE\n";
	}
	else
	{
		message += " HAVE NO SUCH KEY\n";
	}

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
