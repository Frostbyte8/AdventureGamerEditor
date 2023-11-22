#ifndef __INPUT_VALIDATOR_H__
#define __INPUT_VALIDATOR_H__

#include "wxx_wincore.h"
#include "wxx_controls.h"
#include "frost.h"
#include "../compat/std_extras_compat.h"
#include "languagemapper.h"

namespace errorCodes {
    enum ec {
        NoError         = 0,
        OutOfRange      = 1,
        InvalidData     = 2,
        ControlNotFound = 3,
        TooManyChars    = 4,
        StartsWith      = 5,
        EndsWith        = 6,
        NotEnoughChars  = 7,
        GenericError    = 8
    };
}

namespace validatorTypes {
    enum vt {
        None            = 0,
        Integer         = 1,
        String          = 2,
        Generic         = 3
    };
}

// TODO: Base class for all platforms, then sub-class them for OS specific ones
// later.

class InputValidator {

    public:
        InputValidator(const CWnd* wnd, const int& inType) : ctrlWindow(wnd),
                       lastError(errorCodes::NoError), langMessageID(0),
                       langTitleID(0), type(inType) {}

        const CWnd* getWindow() const { return ctrlWindow; }
        const int& getErrorCode() const { return lastError; }
        const int& getType() const { return type; }
        virtual bool validate() = 0;

    protected:

        int lastError;
        int type;
        WORD langMessageID;
        WORD langTitleID;

    private:

        const CWnd* ctrlWindow;
};

class GenericValidator : public InputValidator {

    public:
        GenericValidator() : InputValidator(NULL, validatorTypes::Generic) {}
        GenericValidator(const CWnd* wnd) : InputValidator(wnd, validatorTypes::Generic) {}

        virtual bool validate();
        void setError() { lastError = errorCodes::GenericError; }
        void setLangMsgID(const WORD& langID) { langMessageID = langID; }
        void setLangTitleID(const WORD& langID) { langTitleID = langID; }


};

class IntegerValidator : public InputValidator {

    public:

        IntegerValidator() : InputValidator(NULL, validatorTypes::Integer), minValue(0), maxValue(0) {}

        IntegerValidator(const CWnd* wnd, const LONG& inMinValue,
                         const LONG& inMaxValue) : InputValidator(wnd, validatorTypes::Integer),
                         minValue(inMinValue), maxValue(inMaxValue) {}

        const LONG& getMinValue() const { return minValue; }
        const LONG& getMaxValue() const { return maxValue; }
        virtual bool validate();

    private:

        LONG minValue;
        LONG maxValue;
};

class StringValidator : public InputValidator {

    public:

        StringValidator() : InputValidator(NULL, validatorTypes::String), maxChars(0), minChars(0) {}

            
        StringValidator(const CWnd* wnd, const std::string& inStartsWith, const std::string& inEndsWith, 
                        const int& inMaxChars, const int& inMinChars) : InputValidator(wnd, validatorTypes::String), 
                        maxChars(inMaxChars), minChars(inMinChars) {

            if(!inStartsWith.empty()) {
                startsWith.push_back(inStartsWith);
            }

            if(!inEndsWith.empty()) {
                endsWith.push_back(inEndsWith);
            }

        }

        StringValidator(const CWnd* wnd, const std::vector<std::string>* inStartsWith, 
                        const std::vector<std::string>* inEndsWith, const int& inMaxChars,
                        const int& inMinChars) : InputValidator(wnd, validatorTypes::String), 
                        maxChars(inMaxChars), minChars(inMinChars) {
        
            if(inStartsWith) {
                const size_t startSize = inStartsWith->size();
                startsWith.reserve(startSize);
                for(size_t i = 0; i < startSize; ++i) {
                    startsWith.push_back((*inStartsWith)[i]);
                }
            }

            if(inEndsWith) {
                const size_t endSize = inEndsWith->size();
                endsWith.reserve(endSize);
                for(size_t i = 0; i < endSize; ++i) {
                    endsWith.push_back((*inEndsWith)[i]);
                }
            }
        }

        const std::vector<std::string>& getEndsWith() const { return endsWith; }
        const std::vector<std::string>& getStartsWith() const { return startsWith; }
        const int& getMaxChars() const { return maxChars; }
        const int& getMinChars() const { return minChars; }
        virtual bool validate();

    private:

        std::vector<std::string> startsWith;
        std::vector<std::string> endsWith;
        int minChars;
        int maxChars;
        
};

// TODO: This shouldn't be inline, but removing the inline keyword prevents this from compiling, do this at some point.

inline void processValidatorError(std::string& errorMessage, std::string& errorTitle, const InputValidator* validator) {

    // TODO: Language Mapper probably shouldn't be here.
    LanguageMapper& langMap = LanguageMapper::getInstance();
    const int errorCode = validator->getErrorCode();

    if(validator->getType() == validatorTypes::Integer) {
        const IntegerValidator* intValidator = reinterpret_cast<const IntegerValidator*>(validator);
            
        if(errorCode == errorCodes::OutOfRange) {
            
            // TODO: Update String
            errorMessage = langMap.get("VAL_OutOfRangeText");
            errorTitle = langMap.get("VAL_OutOfRangeTitle");

            // TODO: make this a loop

            size_t pos = errorMessage.find("%d", 0);
            if(pos != std::string::npos) {
                errorMessage.erase(pos, 2);
                std::string intVar = std::to_string(intValidator->getMinValue());
                errorMessage.insert(pos, intVar);
            }

            pos = errorMessage.find("%d", 0);
            if(pos != std::string::npos) {
                errorMessage.erase(pos, 2);
                std::string intVar = std::to_string(intValidator->getMaxValue());
                errorMessage.insert(pos, intVar);
            }

        }
        else if(errorCode == errorCodes::InvalidData) {
            // TODO: Update String
            errorMessage = langMap.get("FileMenu");
            errorTitle = "TODO: Make a title";
        }
    }
    else if(validator->getType() == validatorTypes::String) {
        
        const StringValidator* strValidator = reinterpret_cast<const StringValidator*>(validator);

        if(errorCode == errorCodes::TooManyChars) {
            errorMessage = "String has too many characters. The maximum amount of characters is %d.";

            size_t pos = errorMessage.find("%d", 0);
            if(pos != std::string::npos) {
                errorMessage.erase(pos, 2);
                std::string maxChars = std::to_string(strValidator->getMaxChars());
                errorMessage.insert(pos, maxChars);
            }

        }
        else if(errorCode == errorCodes::NotEnoughChars) {
            
            errorMessage = "String does not have enough chars. The minimum amount of characters is %d.";
            size_t pos = errorMessage.find("%d", 0);

            if(pos != std::string::npos) {
                errorMessage.erase(pos, 2);
                std::string minChars = std::to_string(strValidator->getMinChars());
                errorMessage.insert(pos, minChars);
            }

        }
        else if(errorCode == errorCodes::EndsWith) {
            
            errorMessage = "String must end in one of the following: ";
            
            const std::vector<std::string>& extenVec = strValidator->getEndsWith();
            
            const size_t numExten = extenVec.size();
            
            for(size_t i = 0; i < numExten; ++i) {
                
                errorMessage += extenVec[i].c_str();

                if(i != numExten - 1) {
                    errorMessage += ", ";
                }
                else {
                    errorMessage += ".";
                }
            }
        }

    }

}

#endif // __INPUT_VALIDATOR_H__