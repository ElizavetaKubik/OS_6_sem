#include <iostream>
#include <string>
#include "../OS15_HTAPI/HashTableAPI.h"
#include "../OS15_HTAPI/HashTable.h"
#include "../OS15_HTAPI/Utilities.h"

using namespace std;

#pragma comment(lib,"../Debug/OS15_HTAPI.lib")

int main(int argc, char** argv)
{
	try
	{
		if (argc != 2)
		{
			printf("\nProgram need 1 parameter!");
		}

		HTAPI::OpenApi();

		string file_name = argv[1];

		HT::HashTableData* ht = HTAPI::CreateFromFile(Utilities::GetModulePath() + file_name);

		if (ht != NULL)
		{
			wstring group = ht->HTUsersGroup;
			std::cout << "HashTable - Storage Start filename = " << file_name <<
				", snapshotinterval = " << ht->SnapShotIntervalInSec <<
				", capacity = " << ht->Capacity <<
				", maxkeylength = " << ht->MaxKeyLength <<
				", maxvaluelength = " << ht->MaxValueLength;
			std::wcout << ", group = " << group << std::endl;
		}
		else
		{
			printf("\n error with open");
		}

		std::cout << "Press any key to close... " << std::endl;
		char input;
		std::cin >> input;

		HTAPI::Snap(ht);
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