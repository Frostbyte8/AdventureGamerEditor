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

///----------------------------------------------------------------------------
/// SetWindowTextFromStr - Set the caption of the given control to the given 
/// std::string
/// @param reference to a control.
/// @param reference to a CString to be filled with the new caption string.
/// @param constant reference to the std::string containing the caption.
///----------------------------------------------------------------------------

inline void SetWindowTextFromStr(const std::string& str, CWnd& widget, CString& caption) {
    caption = AtoW(str.c_str(), CP_UTF8);
    widget.SetWindowTextW(caption);
}

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

// TODO: Edit box that can be validated

#endif // __WIN32PP_EXTRAS_H__
