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
        InputValidator(const CWnd* wnd) : ctrlWindow(wnd) {}

        const CWnd* getWindow() { return ctrlWindow; }
        virtual int validate() = 0;

    private:

        const CWnd* ctrlWindow;
};

class IntegerValidator : public InputValidator {

    public:
        IntegerValidator(const CWnd* wnd, const LONG& inMinValue,
                         const LONG& inMaxValue) : InputValidator(wnd),
                         minValue(inMinValue), maxValue(inMaxValue) {}

        virtual int validate();

    private:
        LONG minValue;
        LONG maxValue;
        LONG curMaxValue;
        int lastError;
};

namespace errorCodes {
    enum ec {
        NoError         = 0,
        OutOfRange      = 1,
        InvalidData     = 2,
        ControlNotFound = 3
    };
}

// TODO: Edit box that can be validated

#endif // __WIN32PP_EXTRAS_H__
