#include "editcharacter_tabviews.h"
#include "../model/gamecharacter.h"

#include <algorithm>
#include "shared_functions.h"

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterDescriptionsTab::OnCreate(CREATESTRUCT& cs) {
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText(LanguageConstants::CharDescriptionsGroup, grpDescriptions, caption, langMap);

    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        lblDescriptions[i].Create(*this, 0, SS_SIMPLE);
        txtDescriptions[i].Create(*this, 0, ES_AUTOHSCROLL | WS_TABSTOP);
        txtDescriptions[i].SetExStyle(WS_EX_CLIENTEDGE);

        EOD_SetWindowText(LanguageConstants::CharNameLabel+i, lblDescriptions[i], caption, langMap);
    } 

    return retVal;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterDescriptionsTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

