#include <Windows.h>
#include <iostream>

using namespace std;

namespace Auth
{
    bool checkGroupExisting(LPCWSTR groupName);
    bool checkUserInGroup(LPCWSTR userName, LPCWSTR groupName);
    bool checkCurrentUserGroup(LPCWSTR groupName);
    bool checkUsersCred(LPCWSTR name, LPCWSTR pass);
}