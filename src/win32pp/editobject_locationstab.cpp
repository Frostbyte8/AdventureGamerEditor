#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include <algorithm>

#include "shared_functions.h"

namespace ControlIDs {
    const WORD OnGround         = 101;
    const WORD OnPlayer         = 102;
    const WORD OnCharacter      = 103;
    const WORD UnlocksDoor      = 104;
    const WORD XGroundText      = 105;
    const WORD YGroundText      = 106;
    const WORD XDoorText        = 107;
    const WORD YDoorText        = 108;
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
        parentWindow->madeChange();
    }
    else if (ctrlID == ControlIDs::UnlocksDoor) {
        toggleUnlocksDoor(btnUnlocksDoor.GetCheck() == BST_CHECKED ? TRUE : FALSE);
        parentWindow->madeChange();
    }
    else if(ctrlID == ControlIDs::XGroundText || ctrlID == ControlIDs::YGroundText ||
            ctrlID == ControlIDs::XDoorText || ctrlID == ControlIDs::YDoorText) {
        if(ctrlAction == EN_CHANGE) {
            parentWindow->madeChange();
        }
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

        lblGroundCoord[k].Create(*this, 0, SS_SIMPLE);
        EOD_SetWindowText(LanguageConstants::XCoordLabel+k, lblGroundCoord[k],
                          caption, langMap);

        lblDoorCoord[k].Create(*this, 0, SS_SIMPLE);
        EOD_SetWindowText(LanguageConstants::XCoordLabel+k, lblDoorCoord[k],
                          caption, langMap);

    }

    for(int k = 0; k < 2; ++k) {
        txtGroundCoord[k].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        txtGroundCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        txtGroundCoord[k].LimitText(2); // 0 to 99
        txtGroundCoord[k].EnableWindow(FALSE);
        txtGroundCoord[k].SetDlgCtrlID(ControlIDs::XGroundText + k);
    }


    cbxWhichCharacter.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL | WS_TABSTOP);
    EOD_AddString(LanguageConstants::NoCharacterSelected, cbxWhichCharacter, caption, langMap);

    const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();
    const size_t numCharacters = gameCharacters.size();

    for(size_t j = 0; j < numCharacters; ++j) {
        caption = AtoW((std::to_string(gameCharacters[j].getID()) + ". ").c_str()); 
        caption += AtoW(gameCharacters[j].getName().c_str());
        cbxWhichCharacter.AddString(caption);
    }

    btnUnlocksDoor.Create(*this, 0, BS_AUTOCHECKBOX | WS_TABSTOP);
    EOD_SetWindowText(LanguageConstants::UnlocksDoorAtLabel, btnUnlocksDoor, caption, langMap);
    btnUnlocksDoor.SetDlgCtrlID(ControlIDs::UnlocksDoor);

    for(int k = 0; k < 2; ++k) {
        txtDoorCoord[k].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        txtDoorCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        txtDoorCoord[k].LimitText(2);
        txtDoorCoord[k].SetDlgCtrlID(ControlIDs::XDoorText + k);
    }

    groundCoordValidator[0] = IntegerValidator(&txtGroundCoord[0], 0, gameMap->getWidth() - 1);
    groundCoordValidator[1] = IntegerValidator(&txtGroundCoord[1], 0, gameMap->getHeight() - 1);
    doorCoordVaildator[0] = IntegerValidator(&txtDoorCoord[0], 0, gameMap->getWidth() - 1);
    doorCoordVaildator[1] = IntegerValidator(&txtDoorCoord[1], 0, gameMap->getHeight() - 1);

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

void EditObjectLocationsTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    pageWidth = 0;

    std::vector<CWnd *> controlList;
    
    for(int i = 0; i < 3; ++i) {
        controlList.push_back(&btnLocatedAt[i]);
    }
    
    const size_t clSize = controlList.size();
    
    for(size_t j = 0; j < clSize; ++j) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             controlList[j]->GetWindowTextW().c_str()),
                             pageWidth);
    }

    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();
    LONG btnWidth = windowMetrics.CalculateStringWidth(btnUnlocksDoor.GetWindowTextW().c_str());
    btnWidth += CD.XCHECKBOX + CD.XCHECKBOX_GAP; 

    pageWidth = std::max(btnWidth, pageWidth);

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    pageWidth += CS.XGROUPBOX_MARGIN * 2;
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
            builder.location(gameMap->characterIDFromIndex(charIndex-1));
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

void EditObjectLocationsTab::moveControls(const WindowMetrics& windowMetrics) {
    
    const WindowMetrics::ControlSpacing CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();

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

    // In a number of dialogs, it seems that labels are bottom aligned to the text
    // boxes.

    const int lblCoordWidths = std::max(windowMetrics.CalculateStringWidth(lblDoorCoord[0].GetWindowText().c_str()) * 2,
                                        windowMetrics.CalculateStringWidth(lblDoorCoord[1].GetWindowText().c_str()) * 2);

    const DWORD bothMargins = txtDoorCoord[0].GetMargins();
    const WORD marginOffset = LOWORD(bothMargins) + HIWORD(bothMargins);
    
    // Even though we can only show 2 numbers, we'll make enough space for 3.
    const LONG coordWidth = windowMetrics.CalculateStringWidth(L"000");
    RECT coordRC = {0, 0, coordWidth + marginOffset, 0};
    AdjustWindowRectEx(&coordRC, 0, FALSE, WS_EX_CLIENTEDGE);

    const int txtCoordWidths = coordRC.right + abs(coordRC.left);
    const int lblYOffset = (CD.YTEXTBOX_ONE_LINE_ALONE) - (CD.YLABEL);
    

    lblGroundCoord[0].MoveWindow(cPos.x, cPos.y + lblYOffset,
                                 lblCoordWidths, CD.YLABEL);

    txtGroundCoord[0].MoveWindow(cPos.x + lblCoordWidths, cPos.y,
                                 txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    lblGroundCoord[1].MoveWindow(cPos.x + lblCoordWidths + txtCoordWidths + CS.XRELATED_MARGIN,
                                 cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);

    txtGroundCoord[1].MoveWindow(cPos.x + (lblCoordWidths * 2) + txtCoordWidths + CS.XRELATED_MARGIN, cPos.y,
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

void EditObjectLocationsTab::populateFields(const GameObject& gameObject, const GameMap& gameMap) {

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

    const size_t whichChar = gameMap.characterIndexFromID(gameObject.getCreatureID());

    if(whichChar == ((size_t)-1)) {
        cbxWhichCharacter.SetCurSel(0);
    }
    else {
        cbxWhichCharacter.SetCurSel(whichChar + 1);
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditObjectLocationsTab::validateFields() {

    if(btnLocatedAt[0].GetCheck() == BST_CHECKED) {
        for(int i = 0; i < 2; ++i) {
            if(!groundCoordValidator[i].validate()) {
                return &groundCoordValidator[i];
            }
        }
    }

    if(btnUnlocksDoor.GetCheck() == BST_CHECKED) {
        for(int i = 0; i < 2; ++i) {
            if(!doorCoordVaildator[i].validate()) {
                return &doorCoordVaildator[i];
            }
        }
    }

    return NULL;
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
