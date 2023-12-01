#include "inputvalidator.h"

//=============================================================================
// Validators
//=============================================================================

bool GenericValidator::validate() {

    const CWnd* window = getWindow();

    if(window != NULL && window->IsWindow()) {
    }
    else {
        lastError = errorCodes::ControlNotFound;
        return false;
    }

    lastError = errorCodes::NoError;
    return true;

}

bool IntegerValidator::validate() {

    const CWnd* window = getWindow();

    if(window->IsWindow()) {

        int value = 0;

        try {
            value = std::stol(TtoA(window->GetWindowText()).c_str());
        }
        catch (const std::invalid_argument) {
            lastError = errorCodes::InvalidData;
            return false;
        }
        catch (const std::out_of_range) {
            lastError = errorCodes::OutOfRange;
            return false;
        }

        if(value > maxValue || value < minValue) {
            lastError = errorCodes::OutOfRange;
            return false;
        }
    }
    else {
        lastError = errorCodes::ControlNotFound;
        return false;
    }

    lastError = errorCodes::NoError;
    return true;
}

bool StringValidator::validate() {

    const CWnd* window = getWindow();

    if(!window || !window->IsWindow()) {
        lastError = errorCodes::ControlNotFound;
        return false;
    }

    const int numChars = window->GetWindowTextLength();

    if(maxChars != 0 && numChars > maxChars) {
        lastError = errorCodes::TooManyChars;
        return false;
    }

    if(numChars < minChars) {
        lastError = errorCodes::NotEnoughChars;
        return false;
    }
    
    lastError = errorCodes::NoError;
    const CString inStr = window->GetWindowText();
    CString field;

    if(!startsWith.empty()) {

        const size_t startSize = startsWith.size();

        for(size_t i = 0; i < startSize; ++i) {
            
            field = inStr.Left(startsWith[i].size());

            if(!field.IsEmpty()) {

                field.MakeUpper();

                if(!field.Compare(AtoT(startsWith[i].c_str(), CP_UTF8)) ) {
                    return true;
                }
            }
        }

        lastError = errorCodes::StartsWith;
        return false;
    }

    if(!endsWith.empty()) {

        const size_t endSize = endsWith.size();

        for(size_t i = 0; i < endSize; ++i) {
            
            field = inStr.Right(endsWith[i].size());

            if(!field.IsEmpty()) {

                field.MakeUpper();

                if(!field.Compare(AtoT(endsWith[i].c_str(), CP_UTF8)) ) {
                    return true;
                }
            }
        }

        lastError = errorCodes::EndsWith;
        return false;
    }
 
    return true;
}

