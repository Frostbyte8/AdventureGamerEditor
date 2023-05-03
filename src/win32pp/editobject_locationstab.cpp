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
    EOD_SetWindowText(LanguageConstants::LocationGroupLabel, grpLocations, caption, langMap);

    for(int i = 0; i < 3; ++i) {
        DWORD style = BS_AUTORADIOBUTTON | (i == 0 ? WS_GROUP : 0);
        btnLocatedAt[i].Create(*this, 0, style);
        EOD_SetWindowText(LanguageConstants::OnGroundAtLabel+i, btnLocatedAt[i], caption, langMap);
    }

    for(int k = 0; k < 2; ++k) {
        txtGroundCoord[k].Create(*this, 0, ES_AUTOHSCROLL);
        txtDoorCoord[k].Create(*this, 0, ES_AUTOHSCROLL);
        lblGroundCoord[k].Create(*this, 0, SS_SIMPLE);
        EOD_SetWindowText(LanguageConstants::XCoordLabel+k, lblGroundCoord[k], caption, langMap);
        lblDoorCoord[k].Create(*this, 0, SS_SIMPLE);
        txtGroundCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        txtDoorCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        EOD_SetWindowText(LanguageConstants::XCoordLabel+k, lblDoorCoord[k], caption, langMap);
    }

    cbxWhichCharacter.Create(*this, 0, CBS_DROPDOWN);
    btnUnlocksDoor.Create(*this, 0, BS_AUTOCHECKBOX);
    EOD_SetWindowText(LanguageConstants::UnlocksDoorAtLabel, btnUnlocksDoor, caption, langMap);

    calculatePageWidth();

    return retVal;

}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectLocationsTab::calculatePageWidth() {
    pageWidth = 0;

    std::vector<CWnd *> controlList;
    
    for(int i = 0; i < 3; ++i) {
        controlList.push_back(&btnLocatedAt[i]);
    }
    
    const size_t clSize = controlList.size();
    
    for(size_t j = 0; j < clSize; ++j) {
        pageWidth = std::max(windowMetrics->CalculateStringWidth(
                             controlList[j]->GetWindowTextW().c_str()), pageWidth);
    }

    const WindowMetrics::ControlDimensions CD = windowMetrics->GetControlDimensions();
    LONG buttonWidth = windowMetrics->CalculateStringWidth(btnUnlocksDoor.GetWindowTextW().c_str());
    buttonWidth += CD.XCHECKBOX + CD.XCHECKBOX_GAP; 

    pageWidth = std::max(buttonWidth, pageWidth);
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
    
    CPoint cPos(CS.XWINDOW_MARGIN + CS.XGROUPBOX_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    btnLocatedAt[0].MoveWindow(cPos.x, cPos.y, defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    const int lblCoordWidths = windowMetrics->XDLU2PIX(12);
    const int txtCoordWidths = windowMetrics->XDLU2PIX(24);
    const int lblYOffset = (CD.YTEXTBOX_ONE_LINE_ALONE / 2) - (CD.YLABEL / 2);
    
    lblGroundCoord[0].MoveWindow(cPos.x, cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);
    txtGroundCoord[0].MoveWindow(cPos.x + lblCoordWidths, cPos.y, txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    lblGroundCoord[1].MoveWindow(cPos.x + lblCoordWidths + txtCoordWidths + CS.XRELATED_MARGIN, cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);
    txtGroundCoord[1].MoveWindow(cPos.x + (lblCoordWidths * 2) + txtCoordWidths, cPos.y, txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN);

    btnLocatedAt[1].MoveWindow(cPos.x, cPos.y, defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    btnLocatedAt[2].MoveWindow(cPos.x, cPos.y, defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    cbxWhichCharacter.MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YCOMBOBOX);
    
    cPos.Offset(0, CD.YCOMBOBOX + CS.YRELATED_MARGIN);

    btnUnlocksDoor.MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YCHECKBOX);

    cPos.Offset(0, CD.YCHECKBOX + CS.YRELATED_MARGIN);

    lblDoorCoord[0].MoveWindow(cPos.x, cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);
    txtDoorCoord[0].MoveWindow(cPos.x + lblCoordWidths, cPos.y, txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    lblDoorCoord[1].MoveWindow(cPos.x + lblCoordWidths + txtCoordWidths + CS.XRELATED_MARGIN, cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);
    txtDoorCoord[1].MoveWindow(cPos.x + (lblCoordWidths * 2) + txtCoordWidths, cPos.y, txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE);

    grpLocations.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN, maxGroupBoxWidth, cPos.y);
    
    pageHeight = grpLocations.GetClientRect().Height() + CS.YUNRELATED_MARGIN;
}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectLocationsTab::populateFields(const GameObject& gameObject) {
    const int isLocated = gameObject.getIsLocated();

    btnLocatedAt[isLocated].SetCheck(BST_CHECKED); 

    txtGroundCoord[0].SetWindowText(AtoW(std::to_string(gameObject.getX()).c_str()));
    txtGroundCoord[1].SetWindowText(AtoW(std::to_string(gameObject.getY()).c_str()));

    txtDoorCoord[0].SetWindowText(AtoW(std::to_string(gameObject.getDoorColumn()).c_str()));
    txtDoorCoord[1].SetWindowText(AtoW(std::to_string(gameObject.getDoorRow()).c_str()));

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
