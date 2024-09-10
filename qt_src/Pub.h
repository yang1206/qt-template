#pragma once
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

class pub
{
public:
    static std::string dec2hex(int i, int width);
    static std::string DecIntToHexStr(long long num);
    static std::string DecStrToHexStr(const std::string& str);
};
