#if defined(_MSC_VER) && _MSC_VER <= 1400

#include "std_extras_compat.h"
#include <stdio.h>

bool IsIntegerWithinRange(const std::string& in, const std::string& MIN_INT_STR, const std::string& MAX_INT_STR) {

    const size_t MAX_WITH_SIGN = MIN_INT_STR.length();
    const size_t MAX_WITHOUT_SIGN = MAX_INT_STR.length();

    if(MAX_WITH_SIGN != 1 && in.length() > MAX_WITH_SIGN) {
        return false;
    }
    else if(MAX_WITH_SIGN != 1 && in.length() == MAX_WITH_SIGN) {

        if(in[0] != '-') {
            return false;
        }
        for(size_t i = 1; i <= MAX_WITH_SIGN; i++) {
            if(in[i] > MIN_INT_STR[i]) {
                return false;
            }
            else if(in[i] < MIN_INT_STR[i]) {
                break;
            }
        }
    }
    else if(in.length() == MAX_WITHOUT_SIGN && in[0] != '-') {
        for(size_t i = 0; i < MAX_WITHOUT_SIGN; i++) {
            if(in[i] > MAX_INT_STR[i]) {
                return false;
            }
            else if(in[i] < MAX_INT_STR[i]) {
                break;
            }
        }
    }

    return true;
}

std::string std::to_string(const int& in) {
    const int bufferSize = _scprintf("%d", in);
    char* buffer = new char[bufferSize+1];
    sprintf(buffer, "%d", in);
    std::string output(buffer, bufferSize);
    delete[] buffer;
    return output;
}

int std::stoi(const std::string& in) {

    const std::string MIN_STR = "-2147483648";
    const std::string MAX_STR = "2147483647";

    if(!IsIntegerWithinRange(in, MIN_STR, MAX_STR)) {
        throw std::invalid_argument("Value must be between " + MIN_STR + " and " + MAX_STR + ".");
    }

    return atoi(in.c_str());
}

long std::stol(const std::string& in) {

    const std::string MIN_STR = "-2147483648";
    const std::string MAX_STR = "2147483647";

    if(!IsIntegerWithinRange(in, MIN_STR, MAX_STR)) {
        throw std::invalid_argument("Value must be between " + MIN_STR + " and " + MAX_STR + ".");
    }

    return atol(in.c_str());
}

unsigned long std::stoul(const std::string& in) {

    const std::string MIN_STR = "0";
    const std::string MAX_STR = "4294967295";

    if(!IsIntegerWithinRange(in, MIN_STR, MAX_STR)) {
        throw std::invalid_argument("Value must be between " + MIN_STR + " and " + MAX_STR + ".");
    }

    return atol(in.c_str());
}

#endif //defined(_MSC_VER) && _MSC_VER <= 1400
