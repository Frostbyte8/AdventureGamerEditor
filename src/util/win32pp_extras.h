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

// TODO: Edit box that can be validated

#endif // __WIN32PP_EXTRAS_H__
