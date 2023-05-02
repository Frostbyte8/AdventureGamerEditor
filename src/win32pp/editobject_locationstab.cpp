#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include <algorithm>

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectLocationsTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpLocations.Create(*this, 0, BS_GROUPBOX);

    for(int i = 0; i < 3; ++i) {
        DWORD style = BS_AUTORADIOBUTTON | (i == 0 ? WS_GROUP : 0);
        btnLocatedAt[i].Create(*this, 0, style);
    }

    for(int k = 0; k < 2; ++k) {
        txtGroundCoord[k].Create(*this, 0, ES_AUTOHSCROLL);
        txtDoorCoord[k].Create(*this, 0, ES_AUTOHSCROLL);
        lblGroundCoord[k].Create(*this, 0, SS_SIMPLE);
        lblDoorCoord[k].Create(*this, 0, SS_SIMPLE);
        txtGroundCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        txtDoorCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
    }

    cbxWhichCharacter.Create(*this, 0, CBS_DROPDOWN);
    btnUnlocksDoor.Create(*this, 0, BS_AUTOCHECKBOX);

    calculatePageWidth();

    return retVal;

}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectLocationsTab::calculatePageWidth() {
    pageWidth = 0;
}

///----------------------------------------------------------------------------
/// calculatePageHeight - Finds how tall the tab page needs to be to display
/// the controls
///----------------------------------------------------------------------------

void EditObjectLocationsTab::calculatePageHeight() {
    pageHeight = 0;
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectLocationsTab::moveControls() {
    
    const WindowMetrics::ControlSpacing CS = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics->GetControlDimensions();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);
    
    const CSize defaultRadioSize(maxRowWidth, CD.YRADIOBUTTON);
    
    CPoint cPos(CS.XWINDOW_MARGIN + CS.XRELATED_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    btnLocatedAt[0].MoveWindow(cPos.x, cPos.y, defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    txtGroundCoord[0].MoveWindow(cPos.x, cPos.y, 32, CD.YTEXTBOX_ONE_LINE_ALONE);

    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN);

    btnLocatedAt[1].MoveWindow(cPos.x, cPos.y, defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    btnLocatedAt[2].MoveWindow(cPos.x, cPos.y, defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    cbxWhichCharacter.MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YCOMBOBOX);
    
    cPos.Offset(0, CD.YCOMBOBOX + CS.YRELATED_MARGIN);

    btnUnlocksDoor.MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YCHECKBOX);

    cPos.Offset(0, CD.YCHECKBOX + CS.YRELATED_MARGIN);

    txtDoorCoord[0].MoveWindow(cPos.x, cPos.y, 32, CD.YTEXTBOX_ONE_LINE_ALONE);

    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE);

    grpLocations.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN, maxGroupBoxWidth, cPos.y);

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectLocationsTab::populateFields(const GameObject& gameObject) {

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditObjectLocationsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectLocationsTabPage";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectLocationsTab::PreTranslateMessage(MSG &msg) {

    if(IsDialogMessage(msg)) {
        return TRUE;
    }
    
    return CWnd::PreTranslateMessage(msg);

}
