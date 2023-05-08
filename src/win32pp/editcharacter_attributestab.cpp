#include "editcharacter_tabviews.h"
#include "../model/gamecharacter.h"

#include "shared_functions.h"

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the tab, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterAttributesTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);
    
    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpAttrib.Create(*this, 0, BS_GROUPBOX);

    EOD_SetWindowText(LanguageConstants::CharAttributesGroup, grpAttrib,
                      caption, langMap);

    for(int i = 0; i < 4; ++i) {
        lblAttribType[i].Create(*this, 0, SS_SIMPLE);
        txtAttribType[i].Create(*this, 0, ES_AUTOHSCROLL);
        txtAttribType[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtAttribType[i].LimitText(2);
        EOD_SetWindowText(LanguageConstants::CharAttribEnergy+i, lblAttribType[i], caption, langMap);
        spnAttribType[i].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY | UDS_NOTHOUSANDS |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);
        spnAttribType[i].SetRange(0, 12);
    }

    lblSight.Create(*this, 0, SS_SIMPLE);
    cbxSight.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL | WS_TABSTOP);

    return retVal;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterAttributesTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}
