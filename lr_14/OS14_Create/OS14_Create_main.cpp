#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <string>
#include "../OS14_HTAPI/HashTableAPI.h"
#include "../OS14_HTAPI/HashTable.h"
#include "../OS14_HTAPI/Utilities.h"

using namespace std;

#pragma comment(lib,"../Debug/OS14_HTAPI.lib")

int main(int argc, char** argv)
{
	try
	{
		int capacity = 30, key_length = 64, value_length = 64, snap_time = 60;
		string file_name;
		if (argc == 6)
		{
			int temp;

			temp = atoi(argv[1]);
			capacity = temp;

			temp = atoi(argv[2]);
			snap_time = temp;

			temp = atoi(argv[3]);
			key_length = temp;

			temp = atoi(argv[4]);
			value_length = temp;

			file_name = string(argv[5]);
		}
		else
		{
			printf("\nProgram need 5 parameters! argc: %d.\nDefault parameters are used\n\n", argc);
			file_name = "OS11_map.txt";
		}

		HTAPI::OpenApi();
		
		HT::HashTableData* ht = HTAPI::Create(capacity, snap_time, key_length, value_length, Utilities::GetModulePath() + file_name, L"HT");

		if (ht != NULL)
		{
			wstring group = ht->HTUsersGroup;
			std::cout << "HT - Storage Created filename = " << file_name <<
				", snapshotinterval = " << snap_time <<
				", capacity = " << capacity <<
				", maxkeylength = " << key_length <<
				", maxdatalength = " << value_length;
			std::wcout << ", group = " << group << std::endl;
		}
		else
		{
			printf("\n error");
		}

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

	system("pause");
	return 0;
}