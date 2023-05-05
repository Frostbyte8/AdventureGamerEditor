#ifndef __SHARED_FUNCTIONS_H__
#define __SHARED_FUNCTIONS_H__

// TODO: Maybe Helper functions would be a better name for this?
// TODO: EOD isn't needed as all windows could benefit from these functions.

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/languagemapper.h"

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

#endif // __SHARED_FUNCTIONS_H__
