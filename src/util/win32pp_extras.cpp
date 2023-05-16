#include "win32pp_extras.h"
#include "frost.h"
#include "../compat/std_extras_compat.h"

LRESULT CAnsiEdit::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
    
    switch(msg) {
        case WM_CHAR:
            if(!OnChar(static_cast<wchar_t>(wParam), static_cast<LONG>(lParam))) {
                MessageBeep(0);
                return 0;
            }
            break;
        case WM_PASTE:
            OnPaste();
            return 0;
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
    if(ch == 0x03 || ch == 0x16 || ch == 0x18 ||
       ch == VK_DELETE || ch == VK_BACK || ch == VK_RETURN) {
           // Pass through.
           return true;
    }

    if(!Frost::isCharANSI(ch)) {
        return false;
    }

    return true;
}

///-----------------------------------------------------------------------------
/// OnPaste - Processes the WM_PASTE Message. When something is pasted, it
/// strips out characters that are not part of the ANSI range.
///-----------------------------------------------------------------------------

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
                i++;

            }
            
            // add results.
            this->ReplaceSel(pastedString.c_str(), TRUE);
        }

        CloseClipboard();

    }
}

//=============================================================================
// Validators
//=============================================================================

bool IntegerValidator::validate() {

    const CWnd* window = getWindow();

    if(window->IsWindow()) {

        int value = 0;

        try {
            value = std::stol(WtoA(window->GetWindowText()).c_str());
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

    if(window->IsWindow()) {

        // TODO: Fix warning
        if(maxChars != 0 && window->GetWindowTextLength() > maxChars) {
            lastError = errorCodes::TooManyChars;
            return false;
        }
       
        if(!startsWith.empty() || !endsWith.empty()) {

            const CString inStr = window->GetWindowText();

            CString field = inStr.Left(startsWith.size());
            
            if(!field.IsEmpty()) {
                field.MakeUpper();

                if( field.Compare(AtoW(startsWith.c_str(), CP_UTF8)) ) {
                    lastError = errorCodes::StartsWith;
                    return false;
                }
            }

            field = inStr.Right(endsWith.size());

            if(!field.IsEmpty()) {
                field.MakeUpper();
                if(field.Compare(AtoW(endsWith.c_str(), CP_UTF8))) {
                    lastError = errorCodes::EndsWith;
                    return false;
                }
            }

        }

    }
    else {
        lastError = errorCodes::ControlNotFound;
        return false;
    }

    return true;
}