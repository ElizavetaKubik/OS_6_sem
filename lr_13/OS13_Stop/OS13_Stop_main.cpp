//#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <Windows.h>
//#include <cstdlib>
//#include <string>
//#include "../OS13_HTAPI/Utilities.h"
//#include "../OS13_HTAPI/HashTable.h"
//#include "../OS13_HTAPI/HashTableAPI.h"
//
//#pragma comment(lib,"../Debug/OS13_HTAPI.lib")
//
//using namespace std;
//
//int main(int argc, char* argv[])
//{
//	if (argc == 2)
//	{
//		try
//		{
//			HTAPI::OpenApi();
//
//			string file_name = argv[1];
//
//			HT::HashTableData* ht = HTAPI::Open(file_name);
//
//			BOOL result = HTAPI::Close(ht);
//
//			HTAPI::Close(ht);
//
//			HTAPI::CloseApi();
//
//		}
//		catch (string exeptionMessage)
//		{
//			cout << "exception: " << exeptionMessage << endl;
//		}
//		catch (...)
//		{
//			cout << "unhandled exeption" << endl;
//		}
//	}
//	else
//	{
//		cout << "Error in params\n";
//	}
//
//	system("pause");
//	return 0;
//}