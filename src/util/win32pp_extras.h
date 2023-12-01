#ifndef __WIN32PP_EXTRAS_H__
#define __WIN32PP_EXTRAS_H__

#include <wxx_wincore.h>
#include <wxx_controls.h>
#include <wxx_tab.h>

class CAnsiEdit : public CEdit {

    public:
        CAnsiEdit() : forceTabbing(true), disallowedChars("\"") {}

        void alwaysAllowTabbing(const bool inForceTabbing) {
            forceTabbing = inForceTabbing;
        }

        void setDisallowedChars(std::string disallowChars) {
            disallowedChars = disallowChars;
        }

    protected:
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

    private:

        bool OnChar(const wchar_t& ch, const LONG& keyData);
        void OnPaste();

        bool forceTabbing;
        std::string disallowedChars;
};

inline bool IsScrollBarKey(const int& vk) {
    if(vk > VK_SPACE && vk < VK_SELECT) {
        return true;
    }
    return false;
}

class CTabClassic : public CTab {

    protected:

        // Restore the default Windows behavior for tabs. This ensures
        // that you can use keyboard navigation

        LRESULT OnSetFocus(UINT msg, WPARAM wparam, LPARAM lparam) {
            return FinalWindowProc(msg, wparam, lparam);
        }

        LRESULT OnTCNSelChange(LPNMHDR lp) {
            CTab::OnTCNSelChange(lp);
            SetFocus();
            return 0;
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
    caption = AtoT(str.c_str(), CP_UTF8);
    widget.SetWindowText(caption);
}

#endif // __WIN32PP_EXTRAS_H__
