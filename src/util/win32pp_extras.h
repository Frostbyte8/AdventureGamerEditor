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

class InputValidator {

    public:
        InputValidator(const WORD& inID, const CWnd* wnd) : ctrlID(inID),
                       ctrlWindow(wnd) {}

        const WORD& getCtrlID() { return ctrlID; }
        const CWnd* getWindow() { return ctrlWindow; }

        virtual bool validate() = 0;

    private:
        WORD ctrlID;
        const CWnd* ctrlWindow;
        //std::string lastErrorMessage;
        //std::string lastErrorTitle;
};

class IntegerValidator : public InputValidator {

    public:
        IntegerValidator(const LONG& inMinValue, const LONG& inMaxValue,
                         const WORD& ctrlID, const CWnd* wnd) :
                         InputValidator(ctrlID, wnd),
                         minValue(inMinValue), maxValue(inMaxValue) {}

        virtual bool validate();

    private:
        LONG minValue;
        LONG maxValue;
        LONG curMaxValue;
};

// TODO: Edit box that can be validated

#endif // __WIN32PP_EXTRAS_H__
