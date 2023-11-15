#ifndef __WIN32PP_EXTRAS_H__
#define __WIN32PP_EXTRAS_H__

#include <wxx_wincore.h>
#include <wxx_controls.h>
#include <wxx_tab.h>

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

class CTabClassic : public CTab {

    protected:
        LRESULT OnSetFocus(UINT msg, WPARAM wparam, LPARAM lparam) {
            // By default CTab disables keyboard navigation which is annoying.
            // This repairs that.
            return FinalWindowProc(msg, wparam, lparam);
        }
};

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

#endif // __WIN32PP_EXTRAS_H__
