#include "pub.h"
#include <sstream>

// 十进制整数转十六进制字符串，宽度不足则补0
std::string pub::dec2hex(int i, int width)
{
    std::ostringstream oss;
    oss << std::hex << std::setw(width) << std::setfill('0') << i;
    std::string hexStr = oss.str();

    // 确保结果的长度不超过指定的宽度
    if (hexStr.size() > static_cast<size_t>(width)) {
        hexStr = hexStr.substr(hexStr.size() - width, width);
    }

    return hexStr;
}

// 十进制整数到十六进制字符串的转换，使用循环替代递归
std::string pub::DecIntToHexStr(long long num)
{
    if (num == 0) return "0";

    std::string result;
    while (num > 0) {
        int remainder = num % 16;
        if (remainder < 10)
            result.push_back('0' + remainder);
        else
            result.push_back('A' + remainder - 10);
        num /= 16;
    }

    // 由于我们是从低位到高位构建字符串，最终需要反转
    std::reverse(result.begin(), result.end());
    return result;
}

// 十进制字符串到十六进制字符串的转换
std::string pub::DecStrToHexStr(const std::string& str)
{
    // 将十进制字符串转换为整数
    long long dec = std::stoll(str);
    return DecIntToHexStr(dec);
}