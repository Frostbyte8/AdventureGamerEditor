#ifndef __FROST_H__
#define __FROST_H__

#include <string>
#include <vector>

namespace Frost {
    bool endsWith(const std::string& str, const std::string& val);
    bool startsWith(const std::string& str, const std::string& val);
    std::vector<std::string> split(const std::string& str, const char& delimiter, const size_t& limit = -1);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    std::string ltrim(const std::string& str, const char c = ' ');
    std::string rtrim(const std::string& str, const char c = ' ');
    std::string rtrim(const std::string& str, const std::string& needle = " ");
    inline std::string trim(const std::string& str, const char c = ' ') { return ltrim(rtrim(str, c), c); }
    inline bool isCharANSI(const wchar_t& ch) { if(ch < 32 || ch > 255) { return false; } return true; }
}

#endif // __FROST_H__