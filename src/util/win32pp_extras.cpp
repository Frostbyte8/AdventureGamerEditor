#include "win32pp_extras.h"
#include "frost.h"
#include "../compat/std_extras_compat.h"

LRESULT CAnsiEdit::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
    
    switch(msg) {

        case WM_GETDLGCODE:

            if(wParam == VK_RETURN) {
                if(GetStyle() & ES_WANTRETURN) {
                    return DLGC_WANTMESSAGE;
                }
            }
            else if(forceTabbing) {
                return (WndProcDefault(msg, wParam, lParam) & ~DLGC_WANTALLKEYS);
            }
            break;

        case WM_CHAR:
            if(!OnChar(static_cast<wchar_t>(wParam), static_cast<LONG>(lParam))) {
                MessageBeep(0);
                return 0;
            }
            break;
#ifndef __WIN9X_COMPAT__
        case WM_PASTE:

            OnPaste();
            return 0;
            break;
#endif 

        case WM_KEYDOWN:
            if(wParam == VK_RETURN) {
                return 0;
            }
            break;
    }
    
    return WndProcDefault(msg, wParam, lParam);
}


///-----------------------------------------------------------------------------
/// OnChar - Processes the WM_CHAR message. We do this to prevent the user from
/// entering characters outside the the ANSI range.
/// @param wchar of the character to process
/// @param LONG keyData pertaining to keys pressed.
/// @return bool true if the character is ANSI compatible, false if not.
///-----------------------------------------------------------------------------

bool CAnsiEdit::OnChar(const wchar_t& ch, const LONG& keyData) {

    // Check for special keys first.
    // 01 = CTRL-A, 03 = CTRL-C, 16 = CTRL-V, 18 = CTRL-X
    if(ch == 0x01 || ch == 0x03 || ch == 0x16 || ch == 0x18 ||
       ch == VK_DELETE || ch == VK_BACK || ch == VK_RETURN) {
           // Pass through.
           return true;
    }

    if(!Frost::isCharANSI(ch)) {
        return false;
    }

    for(std::string::iterator it = disallowedChars.begin(); it != disallowedChars.end(); ++it) {
        if(ch == *it) {
            return false;
        }
    }

    return true;
}

///-----------------------------------------------------------------------------
/// OnPaste - Processes the WM_PASTE Message. When something is pasted, it
/// strips out characters that are not part of the ANSI range.
///-----------------------------------------------------------------------------

#ifndef __WIN9X_COMPAT__

void CAnsiEdit::OnPaste() {

    if(IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        
        OpenClipboard(GetHwnd());

        wchar_t* data = NULL;
        HGLOBAL hGlobal;

        std::wstring pastedString;

        // Attempt to get the data held in the clipboard.
        if((hGlobal = reinterpret_cast<wchar_t*>(GetClipboardData(CF_UNICODETEXT)))) {

            // Get the data, and convert it to a wstring.
            data = reinterpret_cast<wchar_t*>(GlobalLock(hGlobal));
            pastedString = std::wstring(data);

            // Strip out non-ANSI characters
            for(size_t i = 0; i < pastedString.length(); ) {
                
                if(!Frost::isCharANSI(pastedString.at(i))) {
                    pastedString.erase(i, 1);
                    continue;
                }
                else {
                    bool strChanged = false;
                    for (std::string::iterator it = disallowedChars.begin(); it != disallowedChars.end(); ++it) {
                        if (pastedString.at(i) == *it) {
                            pastedString.erase(i, 1);
                            strChanged = true;
                            break;
                        }
                    }

                    if(strChanged) {
                        continue;
                    }

                }
                i++;

            }
            
            // add results.
            this->ReplaceSel(pastedString.c_str(), TRUE);
        }

        CloseClipboard();

    }
}
#endif