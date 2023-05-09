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

int EditCharacterMiscTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);
    
    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpLocations.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText(LanguageConstants::CharLocationGroup, grpLocations, caption, langMap);

    for(int i = 0; i < 2; ++i) {
        lblCoords[i].Create(*this, 0, SS_SIMPLE);
        EOD_SetWindowText(LanguageConstants::XCoordLabel, lblCoords[i], caption, langMap);
        txtCoords[i].Create(*this, 0, ES_AUTOHSCROLL);
        txtCoords[i].SetExStyle(WS_EX_CLIENTEDGE);
    }

    grpInventory.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText(LanguageConstants::CharInventoryGroup, grpInventory, caption, langMap);

    lsbInventory.Create(*this, 0, LBS_STANDARD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_DISABLENOSCROLL);

    return retVal;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterMiscTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterMiscTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

void EditCharacterMiscTab::moveControls(const WindowMetrics& windowMetrics) {
    
    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();
    
    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    const int coordWidth = windowMetrics.CalculateStringWidth(L"MM");
    const int txtWidth = windowMetrics.CalculateStringWidth(L"0000");

    for(int i = 0; i < 2; ++i) {
        lblCoords[i].MoveWindow(cPos.x, cPos.y, coordWidth, CD.YTEXTBOX_ONE_LINE_ALONE);
        cPos.Offset(coordWidth, 0);
        txtCoords[i].MoveWindow(cPos.x, cPos.y, txtWidth, CD.YTEXTBOX_ONE_LINE_ALONE);
        cPos.Offset(txtWidth + CS.XRELATED_MARGIN, 0);
    }

    cPos.x = CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN;
    cPos.y += CD.YTEXTBOX_ONE_LINE_ALONE;

    grpLocations.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                            maxGroupBoxWidth, cPos.y);

    cPos.Offset(0, CS.YUNRELATED_MARGIN + CS.YFIRST_GROUPBOX_MARGIN);
    
    lsbInventory.MoveWindow(cPos.x, cPos.y, 
                            maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE * 5);

    cPos.Offset(0, - CS.YFIRST_GROUPBOX_MARGIN);

    grpInventory.MoveWindow(CS.XWINDOW_MARGIN, cPos.y,
                            maxGroupBoxWidth, CS.YFIRST_GROUPBOX_MARGIN + (CD.YTEXTBOX_ONE_LINE_ALONE * 5) + CS.YLAST_GROUPBOX_MARGIN);
    
}

///----------------------------------------------------------------------------
/// populateFields - 
///----------------------------------------------------------------------------

void EditCharacterMiscTab::populateFields(const GameCharacter& gameCharacter, const GameMap& gameMap) {

    CString caption;
    EOD_SetWindowText(std::to_string(gameCharacter.getX()), txtCoords[0], caption);
    EOD_SetWindowText(std::to_string(gameCharacter.getY()), txtCoords[1], caption);

    std::vector<size_t> objectIndices = gameMap.getCharacterInventory(gameCharacter.getID());
    const std::vector<GameObject>& gameObjects = gameMap.getGameObjects();
    const size_t oiSize = objectIndices.size();

    for(size_t i = 0; i < oiSize; ++i) {
        //gameObjects[i].GetName()
    }

}