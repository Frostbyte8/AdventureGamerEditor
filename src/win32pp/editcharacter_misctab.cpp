#include "editcharacter_tabviews.h"
#include "../model/gamecharacter.h"

#include "shared_functions.h"

namespace ControlIDs {
    const WORD XTextCoord   = 101;
    const WORD YTextCoord   = 102;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterMiscTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if(ctrlID >= ControlIDs::XTextCoord && ctrlID <= ControlIDs::YTextCoord) {
            if(notifyCode == EN_CHANGE) {
                parentWindow->madeChange();
            }
        }
    }
    else {
        return FALSE;
    }

    return TRUE;
}

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
    SetWindowTextFromLangMapString("CharLocationGroup", grpLocations, caption, langMap);

    for(int i = 0; i < 2; ++i) {
        lblCoords[i].Create(*this, 0, SS_SIMPLE);
        txtCoords[i].Create(*this, 0, ES_AUTOHSCROLL);
        txtCoords[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtCoords[i].SetDlgCtrlID(ControlIDs::XTextCoord+i);
    }

    SetWindowTextFromLangMapString("XCoord", lblCoords[0], caption, langMap);
    SetWindowTextFromLangMapString("YCoord", lblCoords[1], caption, langMap);

    grpInventory.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("CharInventoryLabel", grpInventory, caption, langMap);

    lsbInventory.Create(*this, 0, LBS_STANDARD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_DISABLENOSCROLL);

    coordValidator[0] = IntegerValidator(&txtCoords[0], 0, gameMap->getWidth() - 1);
    coordValidator[1] = IntegerValidator(&txtCoords[1], 0, gameMap->getHeight() - 1);

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

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterMiscTab::insertData(GameCharacter::Builder& builder) {

    int groundX = 0;
    int groundY = 0;

#ifdef _DEBUG
    // The data was previously validated, so unless a programming error occured
    // this should not fail.
    try {
#endif // _DEBUG
        groundX = std::stoi(WtoA(txtCoords[0].GetWindowText()).c_str());
        groundY = std::stoi(WtoA(txtCoords[1].GetWindowText()).c_str());
#ifdef _DEBUG
    }
    catch(const std::invalid_argument&) {
        assert(0);
    }
    catch(const std::out_of_range&) {
        assert(0);
    }
#endif // _DEBUG 

    builder.location(groundX, groundY);

}

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
    
    // TODO: Fix this, this is incorrect
    pageHeight = grpInventory.GetClientRect().Height() + CS.YUNRELATED_MARGIN;
}

///----------------------------------------------------------------------------
/// populateFields - 
///----------------------------------------------------------------------------

void EditCharacterMiscTab::populateFields(const GameCharacter& gameCharacter) {

    CString caption;
    SetWindowTextFromStr(std::to_string(gameCharacter.getX()), txtCoords[0], caption);
    SetWindowTextFromStr(std::to_string(gameCharacter.getY()), txtCoords[1], caption);

    std::vector<size_t> objectIndices = gameMap->getCharacterInventory(gameCharacter.getID());
    const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
    const size_t oiSize = objectIndices.size();

    for(size_t i = 0; i < oiSize; ++i) {
        lsbInventory.AddString(AtoW(gameObjects[objectIndices[i]].getName().c_str()));
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterMiscTab::validateFields() {

    for(int i = 0; i < 2; ++i) {
        if(!coordValidator[i].validate()) {
            return &coordValidator[i];
        }
    }

    return NULL;
}
