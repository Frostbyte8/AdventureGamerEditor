#include "frost.h"
#include <algorithm>
#include <cctype>
#include <vector>

namespace Frost {

    bool endsWith(const std::string& str, const std::string& val) {

        if(val.length() > str.length()) {
            // String we are searching for is too big to be found.
            return false;
        }
        const size_t pos = str.length() - val.length();

        // Compare the end of the string with the value given
        if(!str.compare(pos, val.length(), val)) {
            return true;
        }
        return false;
    }

    bool startsWith(const std::string& str, const std::string& val) {

        if(val.length() > str.length()) {
            // String we are searching for is too big to be found.
            return false;
        }

        // Compare the beginning of the string with the value given
        if(!str.compare(0, val.length(), val)) {
            return true;
        }
        return false;
    }

    std::vector<std::string> split(const std::string& str, const char& delimiter,
                                   const size_t& limit) {

        size_t startPos         = 0;
        size_t tokensParsed     = 0;
        std::vector<std::string> out;
        std::string token;

        while(true) {

            if(tokensParsed == limit) {
                // Limit hit.
                return out;
            }

            // Find next delimiter
            const size_t pos = str.find(delimiter, startPos);

            if(pos != std::string::npos) {
                
                // Extract the token, add it to the vector, and indicate that
                // we parsed a token.
                token = str.substr(startPos, pos - startPos);
                out.push_back(token);
                startPos = pos + 1;
                tokensParsed++;

            }
            else {
                break; // No more delimiters found
            }
        }

        // Copy the rest of the string as a token, that is, if there is
        // anything to copy.
        if(startPos < str.length()) {
            token = str.substr(startPos, std::string::npos);
            out.push_back(token);
        }

        return out;
    }

    std::string toLower(const std::string& str) {
        std::string out;
        out.resize(str.size());
        // TODO: Why did MSVC accept std::tolower, but not GCC?
        std::transform(str.begin(), str.end(), out.begin(), tolower);
        return out;
    }

    std::string toUpper(const std::string& str) {
        std::string out;
        out.resize(str.size());
        std::transform(str.begin(), str.end(), out.begin(), toupper);
        return out;
    }

    std::string ltrim(const std::string& str, const char c) {

        if(str.length() == 0) {
            return str; // Nothing to trim.
        }

        const size_t firstPos = str.find_first_not_of(c);
        
        if(firstPos == std::string::npos) {
            if(str.at(0) == c) {
                return str.substr(1, std::string::npos);
            }
            return str;
        }

        size_t strlen = str.length();
        return str.substr(firstPos, std::string::npos);
    }

    std::string rtrim(const std::string& str, const char c) {
       
        if(str.length() == 0) {
            return str; // Nothing to trim.
        }

        const size_t lastPos = str.find_last_not_of(c);

        if(lastPos == std::string::npos) {
            if(str.at(str.length() - 1) == c) {
                return str.substr(0, str.length() - 1);
            }
            return str;
        }

        return str.substr(0, lastPos+1);
    }

}
