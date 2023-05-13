#ifndef __SHARED_FUNCTIONS_H__
#define __SHARED_FUNCTIONS_H__

// TODO: Maybe Helper functions would be a better name for this? Maybe shared_win32pp_functions.h
// TODO: EOD isn't needed as all windows could benefit from these functions, not just the dialog.

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/languagemapper.h"

//=============================================================================
// Functions that rely on Langauge Mapper.
//=============================================================================

///----------------------------------------------------------------------------
/// LM_toUTF8 - Converts a Language Mapper string to a CString.
///----------------------------------------------------------------------------

inline CString LM_toUTF8(const unsigned int& ID, const LanguageMapper& langMap) {
    CString retVal = AtoW(langMap.get(ID).c_str(), CP_UTF8);
    return retVal;
}

///----------------------------------------------------------------------------
/// LM_SetWindowText - Set the caption of the given control to the given 
/// languagemap string.
/// @param reference to a control.
/// @param reference to a CString to be filled with the new caption string.
/// @param constant reference to the instance of the Language mapper.
/// @param ID of the string to be used.
///----------------------------------------------------------------------------

inline void EOD_SetWindowText(const unsigned int& ID, CWnd& widget, CString& caption, const LanguageMapper& langMap) {
    caption = AtoW(langMap.get(ID).c_str(), CP_UTF8);
    widget.SetWindowTextW(caption);
}

///----------------------------------------------------------------------------
/// LM_SetWindowText - Set the caption of the given control to the given 
/// std::string
/// @param reference to a control.
/// @param reference to a CString to be filled with the new caption string.
/// @param constant reference to the std::string containing the caption.
///----------------------------------------------------------------------------

inline void EOD_SetWindowText(const std::string& str, CWnd& widget, CString& caption) {
    caption = AtoW(str.c_str(), CP_UTF8);
    widget.SetWindowTextW(caption);
}

///----------------------------------------------------------------------------
/// LM_AddString - Adds a string from the language mapper to the combobox given
/// @param reference to a CCombobox.
/// @param reference to a CString to be filled with the new string.
/// @param constant reference to the instance of the Langauge Mapper.
/// @param ID of the string to be used.
///----------------------------------------------------------------------------

inline void EOD_AddString(const unsigned int& ID, CComboBox& widget, CString& caption, const LanguageMapper& langMap) {
    caption = AtoW(langMap.get(ID).c_str(), CP_UTF8);
    widget.AddString(caption);
}

//=============================================================================
// Helper Functions
//=============================================================================

inline void centerWindowOnCurrentMonitor(const HMONITOR& currentMonitor, CWnd& window) {
    
    RECT rc = window.GetClientRect();
    
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(currentMonitor, &monitorInfo);

    AdjustWindowRectEx(&rc, window.GetStyle(), FALSE, window.GetExStyle());

    // Calculate Where the window is positioned, then offset it to where the monitor is.

    const CPoint windowPos((abs(monitorInfo.rcWork.right - monitorInfo.rcWork.left) / 2) - ((rc.right + abs(rc.left)) / 2) + monitorInfo.rcMonitor.left,
                           (abs(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top) / 2) - ((rc.bottom + abs(rc.top)) / 2) + monitorInfo.rcMonitor.top);

    window.SetWindowPos(0, windowPos.x, windowPos.y, 0, 0, SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_NOREPOSITION);

}

#endif // __SHARED_FUNCTIONS_H__
