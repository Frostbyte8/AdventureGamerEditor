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
    EOD_SetWindowText(LanguageConstants::CharSightLabel, lblSight, caption, langMap);

    cbxSight.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL | WS_TABSTOP);

    for(int i = 0; i < 3; ++i) {
        EOD_AddString(LanguageConstants::CharSightNormal+i, cbxSight, caption, langMap);
    }

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

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::calculatePageWidth(const WindowMetrics& windowMetrics) {
    
    pageWidth = 0;

    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();

    //TODO: Size the 5 labels. 
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::moveControls(const WindowMetrics& windowMetrics) {
    
    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();
    
    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultTextSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
        CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    for(int i = 0; i < 4; ++i) {

        lblAttribType[i].MoveWindow(cPos.x, cPos.y, 
                                    defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtAttribType[i].MoveWindow(cPos.x, cPos.y,
                                    defaultTextSize.cx, defaultTextSize.cy);

        cPos.Offset(0, defaultTextSize.cy + CS.YLABELASSOC_MARGIN);

        spnAttribType[i].SetBuddy(txtAttribType[i].GetHwnd());
    }

    lblSight.MoveWindow(cPos.x, cPos.y,
                        defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

    cbxSight.MoveWindow(cPos.x, cPos.y,
                        defaultTextSize.cx, CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, CD.YDROPDOWN);

    grpAttrib.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                         maxGroupBoxWidth, cPos.y);


    pageHeight = grpAttrib.GetClientRect().Height() + CS.YUNRELATED_MARGIN;

}

///----------------------------------------------------------------------------
/// populateFields - 
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::populateFields(const GameCharacter& gameCharacter, const GameMap& gameMap) {

    for(int i = 0; i < 4; ++i) {
        spnAttribType[i].SetPos(gameCharacter.getAttribute(i));
    }

    cbxSight.SetCurSel(gameCharacter.getSight());

}
