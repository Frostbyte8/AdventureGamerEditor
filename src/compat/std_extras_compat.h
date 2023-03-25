#ifndef __STD_EXTRAS_COMPAT_H__
#define __STD_EXTRAS_COMPAT_H__

// Adds a few features so newer compilers can compile it with less changes.

#include <string>

#if defined(_MSC_VER)
#ifdef _WIN32

    #if _MSC_VER <= 1500

    namespace std {

        std::string to_string(const int& in);
        
        // For these, we'll just assume that idx is 0 and base is 10 since
        // nothing needs that functionality right now.
        
        int stoi(const std::string& in);
        long stol(const std::string& in);
        unsigned long stoul(const std::string& in);

    }

    #endif //_MSC_VER < 1500
#endif // _WIN32
#endif // _MSC_VER

#endif // __STD_EXTRAS_COMPAT_H__
