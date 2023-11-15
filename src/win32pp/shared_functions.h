#ifndef __SHARED_FUNCTIONS_H__
#define __SHARED_FUNCTIONS_H__

// TODO: Maybe Helper functions would be a better name for this? Maybe shared_win32pp_functions.h

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/languagemapper.h"
#include "../util/win32pp_extras.h"

///----------------------------------------------------------------------------
/// LM_toUTF8 - Converts a Language Mapper string to a CString.
///----------------------------------------------------------------------------

inline CString LM_toUTF8(const std::string& ID, const LanguageMapper& langMap) {
    CString retVal = AtoW(langMap.get(ID).c_str(), CP_UTF8);
    return retVal;
}

///----------------------------------------------------------------------------
/// SetWindowTextFromLangMapString - Set the caption of the given control to
/// the given languagemap string.
/// @param ID of the string to be used.
/// @param reference to a control having it's text changed.
/// @param reference to a CString to be filled with the new caption string.
/// @param constant reference to the instance of the Language mapper.
///----------------------------------------------------------------------------

inline void SetWindowTextFromLangMapString(const std::string& ID, CWnd& widget,CString& caption, const LanguageMapper& langMap) {
    SetWindowTextFromStr(langMap.get(ID), widget, caption);
}

///----------------------------------------------------------------------------
/// AddStringFromLangMap - Adds a string from the language mapper to the combobox given
/// @param ID of the string to be used.
/// @param reference to a CCombobox.
/// @param reference to a CString to be filled with the new string.
/// @param constant reference to the instance of the Langauge Mapper.
///----------------------------------------------------------------------------

inline void AddStringFromLangMap(const std::string& ID, CComboBox& widget, CString& caption, const LanguageMapper& langMap) {
    caption = AtoW(langMap.get(ID).c_str(), CP_UTF8);
    widget.AddString(caption);
}

//=============================================================================
// Helper Functions
//=============================================================================

bool dlgOnBrowseForMedia(const CWnd& parentDialog, CEdit& txtOut, const bool findIcon);

void createDefaultDialogButtons(const CWnd& parentDialog, CButton (&dialogButtons)[3], const bool hasApply);

void centerWindowOnCurrentMonitor(const HMONITOR& currentMonitor, CWnd& window);

void DrawTileSelectionBox(CMemDC& inDC, const int& xOffset, const int& yOffset,
                          const int& selectionWidth, const int selectionHeight,
                          const int& borderWidth);

#endif // __SHARED_FUNCTIONS_H__