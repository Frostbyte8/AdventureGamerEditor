#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include <algorithm>

#include "shared_functions.h"

namespace ControlIDs {
    const WORD OnGround         = 101;
    const WORD OnPlayer         = 102;
    const WORD OnCharacter      = 103;
    const WORD UnlocksDoor      = 104;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Process the WM_COMMAND message. Refer to to the Win32++
/// documentation for more details.
///----------------------------------------------------------------------------

BOOL EditObjectLocationsTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(!lParam) {
        return FALSE;
    }

    const WORD ctrlID = LOWORD(wParam);
    const WORD ctrlAction = HIWORD(wParam);

    if(ctrlID >= ControlIDs::OnGround && ctrlID <= ControlIDs::OnCharacter) {
        locatedAtChanged(ctrlID, ctrlAction);
    }
    else if (ctrlID == ControlIDs::UnlocksDoor) {
        toggleUnlocksDoor(btnUnlocksDoor.GetCheck() == BST_CHECKED ? TRUE : FALSE);
    }
    else {
        return FALSE;
    }

    return TRUE;
}

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectLocationsTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpLocations.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText(LanguageConstants::LocationGroupLabel, grpLocations,
                      caption, langMap);

    for(int i = 0; i < 3; ++i) {

        DWORD style = BS_AUTORADIOBUTTON | (i == 0 ? WS_GROUP : 0);
        btnLocatedAt[i].Create(*this, 0, style);
        btnLocatedAt[i].SetDlgCtrlID(ControlIDs::OnGround+i);

        EOD_SetWindowText(LanguageConstants::OnGroundAtLabel+i,
                          btnLocatedAt[i], caption, langMap);
    }

    for(int k = 0; k < 2; ++k) {

        txtGroundCoord[k].Create(*this, 0, ES_AUTOHSCROLL);
        txtDoorCoord[k].Create(*this, 0, ES_AUTOHSCROLL);
        lblGroundCoord[k].Create(*this, 0, SS_SIMPLE);
        EOD_SetWindowText(LanguageConstants::XCoordLabel+k, lblGroundCoord[k],
                          caption, langMap);

        lblDoorCoord[k].Create(*this, 0, SS_SIMPLE);
        txtGroundCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        txtDoorCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        EOD_SetWindowText(LanguageConstants::XCoordLabel+k, lblDoorCoord[k],
                          caption, langMap);

        txtDoorCoord[k].LimitText(2); // 0 to 99
        txtDoorCoord[k].LimitText(2);

    }

    cbxWhichCharacter.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL);
    EOD_AddString(LanguageConstants::NoCharacterSelected, cbxWhichCharacter, caption, langMap);

    const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();
    const size_t numCharacters = gameCharacters.size();

    for(size_t j = 0; j < numCharacters; ++j) {
        caption = AtoW((std::to_string(gameCharacters[j].getID()) + ". ").c_str()); 
        caption += AtoW(gameCharacters[j].getName().c_str());
        cbxWhichCharacter.AddString(caption);
    }

    cbxWhichCharacter.SetCurSel(0);

    btnUnlocksDoor.Create(*this, 0, BS_AUTOCHECKBOX);
    EOD_SetWindowText(LanguageConstants::UnlocksDoorAtLabel, btnUnlocksDoor, caption, langMap);
    btnUnlocksDoor.SetDlgCtrlID(ControlIDs::UnlocksDoor);

    calculatePageWidth();

    return retVal;

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

//=============================================================================
// Public Functions
//=============================================================================

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
                             controlList[j]->GetWindowTextW().c_str()),
                             pageWidth);
    }

    const WindowMetrics::ControlDimensions CD = windowMetrics->GetControlDimensions();
    LONG btnWidth = windowMetrics->CalculateStringWidth(btnUnlocksDoor.GetWindowTextW().c_str());
    btnWidth += CD.XCHECKBOX + CD.XCHECKBOX_GAP; 

    pageWidth = std::max(btnWidth, pageWidth);
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditObjectLocationsTab::insertData(GameObject::Builder& builder) {
    
    if(btnLocatedAt[0].GetCheck() == BST_CHECKED) {

        int groundX = std::stoi(WtoA(txtGroundCoord[0].GetWindowText()).c_str());
        int groundY = std::stoi(WtoA(txtGroundCoord[1].GetWindowText()).c_str());
        builder.location(groundX, groundY);

    }
    else if(btnLocatedAt[1].GetCheck() == BST_CHECKED) {

        builder.location();

    }
    else {

        const int charIndex = cbxWhichCharacter.GetCurSel();

        if(charIndex == 0) {
            // Default to on the ground at 0, 0
            builder.location(0, 0);
        }
        else {
            // Map Index to ID
            // charID = gameMap->characterIndexToID(charIndex);
        }
    }

    if(btnUnlocksDoor.GetCheck() == BST_CHECKED) {

        int doorX = std::stoi(WtoA(txtDoorCoord[0].GetWindowText()).c_str());
        int doorY = std::stoi(WtoA(txtDoorCoord[1].GetWindowText()).c_str());   

        // Only set this if the value is valid
        if((doorX > -1 && doorX <= gameMap->getWidth()) &&
           (doorY > -1 && doorY <= gameMap->getHeight())) {

            builder.flags2(builder.getFlags2() | GameObjectFlags2::Key);
            builder.doorColumn(doorX);
            builder.doorRow(doorY);

        }

    }

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

    btnLocatedAt[0].MoveWindow(cPos.x, cPos.y,
                               defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    // TODO: The Labels and textboxes are not properly Aligned, nor are they
    // the correct size.

    const int lblCoordWidths = windowMetrics->XDLU2PIX(12);
    const int txtCoordWidths = windowMetrics->XDLU2PIX(24);
    const int lblYOffset = (CD.YTEXTBOX_ONE_LINE_ALONE / 2) - (CD.YLABEL / 2);
    

    lblGroundCoord[0].MoveWindow(cPos.x, cPos.y + lblYOffset,
                                 lblCoordWidths, CD.YLABEL);

    txtGroundCoord[0].MoveWindow(cPos.x + lblCoordWidths, cPos.y,
                                 txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    lblGroundCoord[1].MoveWindow(cPos.x + lblCoordWidths + txtCoordWidths + CS.XRELATED_MARGIN,
                                 cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);

    txtGroundCoord[1].MoveWindow(cPos.x + (lblCoordWidths * 2) + txtCoordWidths, cPos.y,
                                 txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN);

    btnLocatedAt[1].MoveWindow(cPos.x, cPos.y,
                               defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    btnLocatedAt[2].MoveWindow(cPos.x, cPos.y,
                               defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    cbxWhichCharacter.MoveWindow(cPos.x, cPos.y, maxRowWidth, 
                                 CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, CD.YCOMBOBOX + CS.YRELATED_MARGIN);

    btnUnlocksDoor.MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YCHECKBOX);

    cPos.Offset(0, CD.YCHECKBOX + CS.YRELATED_MARGIN);

    lblDoorCoord[0].MoveWindow(cPos.x, cPos.y + lblYOffset,
                               lblCoordWidths, CD.YLABEL);

    txtDoorCoord[0].MoveWindow(cPos.x + lblCoordWidths, cPos.y,
                               txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    lblDoorCoord[1].MoveWindow(cPos.x + lblCoordWidths + txtCoordWidths + CS.XRELATED_MARGIN,
                               cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);

    txtDoorCoord[1].MoveWindow(cPos.x + (lblCoordWidths * 2) + txtCoordWidths, cPos.y,
                               txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE);

    grpLocations.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                            maxGroupBoxWidth, cPos.y);
    
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
    locatedAtChanged(isLocated + ControlIDs::OnGround, BN_CLICKED); 

    txtGroundCoord[0].SetWindowText(AtoW(std::to_string(
                                    gameObject.getX()).c_str()));

    txtGroundCoord[1].SetWindowText(AtoW(std::to_string(
                                    gameObject.getY()).c_str()));

    txtDoorCoord[0].SetWindowText(AtoW(std::to_string(
                                  gameObject.getDoorColumn()).c_str()));

    txtDoorCoord[1].SetWindowText(AtoW(std::to_string(
                                  gameObject.getDoorRow()).c_str()));

    const BOOL unlocksDoor = gameObject.getFlags2() & GameObjectFlags2::Key ? TRUE : FALSE;

    toggleUnlocksDoor(unlocksDoor);
    btnUnlocksDoor.SetCheck(unlocksDoor);

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return 0 if no errors are found, and if an error is found, returns the ID
/// of the control that caused the validation error.
///----------------------------------------------------------------------------

WORD EditObjectLocationsTab::validateFields() {

    const int mapWidth = gameMap->getWidth() - 1;
    const int mapHeight = gameMap->getHeight() - 1;

    // TODO: Given how many times this has popped up while testing, this
    // appears to be broken.

    if(btnLocatedAt[0].GetCheck() == BST_CHECKED) {

        const int groundX = std::stoi(WtoA(txtGroundCoord[0].GetWindowText()).c_str());
        const int groundY = std::stoi(WtoA(txtGroundCoord[1].GetWindowText()).c_str());

        if(groundX < 0 || groundX > mapWidth || groundY < 0 || groundY > mapHeight) {
            MessageBox(L"Object cannot be placed outside of map bounds",
                       L"Validation Error", MB_OK | MB_ICONERROR);
            return 1;
        }

    }

    if(btnUnlocksDoor.GetCheck() == BST_CHECKED) {

        const int doorX = std::stoi(WtoA(txtDoorCoord[0].GetWindowText()).c_str());
        const int doorY = std::stoi(WtoA(txtDoorCoord[1].GetWindowText()).c_str());

        if(doorX < 0 || doorX > mapWidth || doorY < 0 || doorY > mapHeight) {
            MessageBox(L"Object cannot be told to unlock a door outside the map bounds",
                       L"Validation Error", MB_OK | MB_ICONERROR);
            return 1;
        }

        // TODO: Check if a door exists at the given coordinates, and if none
        // exists, ask the user if this is correct.

    }


    return 0;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// locatedAtChanged - Change what is able to be set after the location of the
/// object has been changed.
/// @param an integer referring to the control ID of the option that was chosen.
/// @param an integer that indicated the control's notification code
///----------------------------------------------------------------------------

void EditObjectLocationsTab::locatedAtChanged(const WORD& ctrlID, const WORD& ctrlAction) {

    const int which = ctrlID - ControlIDs::OnGround;
    const bool isChecked = btnLocatedAt[which].GetCheck() == BST_CHECKED ? true : false; 

    if(ctrlID == ControlIDs::OnGround && isChecked) {
        txtGroundCoord[0].EnableWindow(TRUE);
        txtGroundCoord[1].EnableWindow(TRUE);
    }
    else {
        txtGroundCoord[0].EnableWindow(FALSE);
        txtGroundCoord[1].EnableWindow(FALSE);
    }

    if(ctrlID == ControlIDs::OnCharacter && isChecked) {
        cbxWhichCharacter.EnableWindow(TRUE);
    }
    else {
        cbxWhichCharacter.EnableWindow(FALSE);
    }

}

///----------------------------------------------------------------------------
/// toggleUnlockDoor - Change whether the unlock door textboxes are enabled
/// or not based on the state of the unlocks door checkbox
/// @param a bool indicating whether or not the object unlocks a door.
///----------------------------------------------------------------------------

void EditObjectLocationsTab::toggleUnlocksDoor(const BOOL& doesUnlock) {
    txtDoorCoord[0].EnableWindow(doesUnlock);
    txtDoorCoord[1].EnableWindow(doesUnlock);
}
