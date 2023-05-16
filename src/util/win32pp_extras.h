#ifndef __WIN32PP_EXTRAS_H__
#define __WIN32PP_EXTRAS_H__

#include "wxx_wincore.h"
#include "wxx_controls.h"

class CAnsiEdit : public CEdit {

    protected:
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool OnChar(const wchar_t& ch, const LONG& keyData);
        void OnPaste();
};

inline bool IsScrollBarKey(const int& vk) {
    if(vk > VK_SPACE && vk < VK_SELECT) {
        return true;
    }
    return false;
}

namespace errorCodes {
    enum ec {
        NoError         = 0,
        OutOfRange      = 1,
        InvalidData     = 2,
        ControlNotFound = 3,
        TooManyChars    = 4,
        StartsWith      = 5,
        EndsWith        = 6
    };
}

namespace validatorTypes {
    enum vt {
        None            = 0,
        Integer         = 1,
        String          = 2
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

        StringValidator() : InputValidator(NULL, validatorTypes::String), startsWith(""),
                                           endsWith(""), maxChars(0) {}

        StringValidator(const CWnd* wnd, const std::string inStartsWith,
                        const std::string& inEndsWith, const size_t& inMaxChars) : InputValidator(wnd, validatorTypes::String),
                        startsWith(inStartsWith), endsWith(inEndsWith), maxChars(inMaxChars) {}

        const std::string& getEndsWith() const { return endsWith; }
        const std::string& getStartsWith() const { return startsWith; }
        const size_t& getMaxChars() const { return maxChars; }
        virtual bool validate();

    private:

        std::string startsWith;
        std::string endsWith;
        size_t maxChars;
        
};

// TODO: Edit box that can be validated

#endif // __WIN32PP_EXTRAS_H__
