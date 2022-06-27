#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <string>
#include "./Utilities.h"
#include "./HashTable.h"

using namespace std;

void InsertTestData(HT::HashTable& inHashTable)
{
	string key = "test_key_";
	string value = "test_value_";
	string temp_key;
	string temp_value;
	cout << "\ninserting...\n";

	for (int i = 0; i < 10 && i <= inHashTable.GetCapacity(); i++)
	{
		temp_key = key + to_string(i);
		temp_value = value + to_string(i);
		if (inHashTable.Insert(temp_key.c_str(), temp_key.length(), temp_value.c_str(), temp_value.length()))
		{
			cout << "Test element was added: "; inHashTable.Print(temp_key.c_str());
			cout << endl;
		}
		else
		{
			cout << "Test element adding was denied: " << temp_key.c_str();
			cout << endl;
		}
	}
	cout << "\nResult: \n";
	inHashTable.PrintAll();
}

void CrudTest(HT::HashTable& inHashTable)
{
	cout << "Hash table:\n";
	inHashTable.PrintAll();

	char choice;
	string cin_key;
	string cin_value;


	while (true)
	{
		try
		{
			cout << "\n\n-----------------------\n";
			cout << "1: Insert.\n";
			cout << "2: Get.\n";
			cout << "3: Update.\n";
			cout << "4: Delete.\n";
			cout << "5: Clear Hash Table.\n";
			cout << "9: Print Hash Table.\n";
			cout << "0: Exit.\n";
			cout << "-----------------------\n\n";
			cout << "Choise: ";
			cin >> choice;
			switch (choice)
			{
			case '0': break;
			case '1':
			{
				cout << "key: "; cin >> cin_key;
				cout << "value: "; cin >> cin_value;

				if (inHashTable.Insert(cin_key.c_str(), cin_key.length(), cin_value.c_str(), cin_value.length()))
				{
					cout << "Success: "; inHashTable.Print(cin_key.c_str());
				}
				else
				{
					cout << "Element adding denied: ";
				}
				break;
			}
			case '2':
			{
				cout << "key: "; cin >> cin_key;

				inHashTable.Print(cin_key.c_str());
				break;
			}
			case '3':
			{
				cout << "key: "; cin >> cin_key;
				cout << "new value: "; cin >> cin_value;

				if (inHashTable.Update(cin_key.c_str(), cin_value.c_str(), cin_value.length()))
				{
					cout << "Success: "; inHashTable.Print(cin_key.c_str());
				}
				else
				{
					cout << "Element update denied: ";
				}
				break;
			}
			case '4':
			{
				cout << "key: "; cin >> cin_key;

				if (inHashTable.Delete(cin_key.c_str()))
				{
					cout << "Success: "; cout << cin_key.c_str();
				}
				else
				{
					cout << "Element deletion denied: ";
				}
				break;
			}
			case '5':
			{
				inHashTable.Clear();
				cout << "\nSuccess.\n";
				break;
			}
			case '9':
			{
				inHashTable.PrintAll();
				break;
			}

			default:
			{
				cout << "unhandled command: " << choice << endl;
				break;
			}
			}

			if (choice == '0')
			{
				break;
			}
		}
		catch (string exeptionMessage)
		{
			cout << "exception: " << exeptionMessage << endl;
		}
		catch (const char* exeption_string)
		{
			cout << "exception: " << exeption_string << endl;
		}
		catch (...)
		{
			cout << "unhandled exeption" << endl;
		}
	}
}


void TestHTAPI()
{

	try
	{
		//Create new
		// 
		HT::HashTable test_hash_table(20, 30, 20, 20, Utilities::GetModulePath() + "map.txt");
		InsertTestData(test_hash_table);

		//Open
		//
		//string path = string(Utilities::GetModulePath() + "map.txt");
		//HT::HashTable test_hash_table(path.c_str());

		cout << endl << "test" << endl;

		char choice;
		while (true)
		{
			try
			{
				cout << "\n\n-----------------------\n";
				cout << "1: CRUD test.\n";
				cout << "2: Add test data.\n";
				cout << "3: Clear console.\n";
				cout << "0: Exit.\n";
				cout << "-----------------------\n\n";

				cout << "Choise: "; cin >> choice;
				switch (choice)
				{
				case '0': break;
				case '1':
				{
					CrudTest(test_hash_table);
					break;
				}
				case '2':
				{
					InsertTestData(test_hash_table);
					break;
				}
				case '3':
				{
					system("CLS");
					break;
				}
				default:
				{
					cout << "unhandled command: " << choice << endl;
					break;
				}
				}

				if (choice == '0')
				{
					test_hash_table.Close();
					break;
				}
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
int main()
{
	TestHTAPI();
	system("pause");
	return 0;
}