#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <string>
#include "../HTAPI/Utilities.h"
#include "../HTAPI/HashTable.h"

using namespace std;

#pragma comment(lib,"../Debug/HTAPI.lib")

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		try
		{
			string file_name = Utilities::GetModulePath() + argv[1];
			
			HT::HashTableData* hash_table_data = HT::CreateHT_FromFile(file_name.c_str());

			std::cout << "HashTable - Storage Start filename = " << file_name <<
				", snapshotinterval = " << HT::GetSnapShotInterval(hash_table_data) <<
				", capacity = " << HT::GetCapacity(hash_table_data) <<
				", maxkeylength = " << HT::GetMaxKeyLength(hash_table_data) <<
				", maxvaluelength = " << HT::GetMaxValueLength(hash_table_data) << std::endl;

			std::cout << "Press any key to close... " << std::endl;

			char input;
			std::cin >> input;
			HT::Snap(hash_table_data);
			HT::Close(hash_table_data);
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
		std::cout << "Not enough params" << std::endl;
	}
	system("pause");
	return 0;
}