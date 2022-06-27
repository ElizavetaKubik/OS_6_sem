#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>

#include "HT_Service.h"
using namespace std;

int main()
{
    WCHAR ServiceName[] = { SERVICENAME };

    SERVICE_TABLE_ENTRY ServiceStartTable[] =
    {
        {ServiceName, ServiceMain},
        {NULL,NULL}
    };


    if (!StartServiceCtrlDispatcher(ServiceStartTable))
    {
        std::cout << "Successful service start\n";
        trace("Successful service start\n", std::ofstream::out);
    }
    else
    {
        std::cout << "Service start error\n";
        trace("Service start error", std::ofstream::out);
    }

    system("pause");
    return 0;
}
