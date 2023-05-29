#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include "../util/languagemapper.h"
#include <algorithm>

#include "shared_functions.h"

namespace ControlIDs {
    const WORD MasterKey    = 101;
    const WORD Invisible    = 102;
    const WORD Climber      = 103;
    const WORD Protection   = 104;
    const WORD Torch        = 105;
    const WORD Worn         = 106;
    const WORD Fixed        = 107;
    const WORD Money        = 108;
    const WORD UsesBox      = 109;
    const WORD MoneyBox     = 110;
    const WORD UsedWith     = 111;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Process the WM_COMMAND message. Refer to to the Win32++
/// documentation for more details.
///----------------------------------------------------------------------------

BOOL EditObjectQualitiesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(!lParam) {
        return FALSE;
    }

    const WORD ctrlID = LOWORD(wParam);
    const WORD ctrlAction = HIWORD(wParam);

    if(ctrlID >= ControlIDs::MasterKey && ctrlID <= ControlIDs::Money) {
        flagsChanged(ctrlID, ctrlAction); 
        parentWindow->madeChange();
    }
    else if(ctrlID == ControlIDs::UsesBox || ctrlID == ControlIDs::MoneyBox) {
        if(ctrlAction == EN_KILLFOCUS) {
            updatePropertyValue(ctrlID);
        }
        else if(ctrlAction == EN_CHANGE) {
            parentWindow->madeChange();
        }
    }
    else if(ctrlID == ControlIDs::UsedWith) {
        if(ctrlAction == CBN_SELCHANGE) {
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

int EditObjectQualitiesTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);
    
    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpFlags.Create(*this, 0, BS_GROUPBOX);

    EOD_SetWindowText("FileMenu", grpFlags,
                      caption, langMap);

    for(int k = 0; k < GameObjectFlags1::NumFlags; ++k) {

        btnFlags[k].Create(*this, 0, BS_AUTOCHECKBOX | WS_TABSTOP);
        btnFlags[k].SetDlgCtrlID(ControlIDs::MasterKey + k);
        EOD_SetWindowText("FileMenu", btnFlags[k],
                          caption, langMap);

    }

    for(int i = 0; i < 2; ++i) {

        lblProperties[i].Create(*this, 0, SS_SIMPLE);
        txtProperties[i].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        txtProperties[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtProperties[i].LimitText(5);

        EOD_SetWindowText("FileMenu", lblProperties[i],
                          caption, langMap);

        // TODO: Why does WS_VISBLE have to be set? Possible bug?
        spnProperties[i].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY | UDS_NOTHOUSANDS |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

        txtProperties[i].SetDlgCtrlID(ControlIDs::UsesBox+i);
       
    }

    grpProperties.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText("FileMenu", grpProperties, caption, langMap);

    lblProperties[2].Create(*this, 0, SS_SIMPLE);
    EOD_SetWindowText("FileMenu", lblProperties[2],
                      caption, langMap);

    cbxUsedWith.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL | WS_TABSTOP);
    EOD_AddString("FileMenu", cbxUsedWith,
                  caption, langMap);

    const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
    const size_t numObjects = gameObjects.size();

    for(size_t j = 0; j < numObjects; ++j) {
        caption = AtoW((std::to_string(gameObjects[j].getID()) + ". ").c_str()); 
        caption += AtoW(gameObjects[j].getName().c_str());
        cbxUsedWith.AddString(caption);
        cbxUsedWith.SetDlgCtrlID(ControlIDs::UsedWith);
    }

    spnProperties[0].SetRange(GameObjectConstants::MaxMonetaryValue,
                              GameObjectConstants::MinMonetaryValue);
                              

    spnProperties[1].SetRange(GameObjectConstants::MaxNumUses,
                              GameObjectConstants::MinNumUses);
    
    // TODO: Money constants need to be in their own namespace. See comment in EditCharacterDialog for more information.

    moneyValidator = IntegerValidator(&txtProperties[0], GameObjectConstants::MinMonetaryValue, GameObjectConstants::MaxMonetaryValue);
    usesValidator = IntegerValidator(&txtProperties[1], GameObjectConstants::MinNumUses, GameObjectConstants::MaxNumUses);

    return retVal;

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectQualitiesTabPage";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectQualitiesTab::PreTranslateMessage(MSG &msg) {

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

void EditObjectQualitiesTab::calculatePageWidth(const WindowMetrics& windowMetrics) {
    
    pageWidth = 0;

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    const LONG checkboxWidth = CD.XCHECKBOX + CD.XCHECKBOX_GAP;
    
    for(int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             btnFlags[i].GetWindowTextW().c_str()) + checkboxWidth,
                             pageWidth);
    }

    std::vector<CWnd *> controlList;
    controlList.reserve(2);
        
    for(int k = 0; k < 2; ++k) {
        controlList.push_back(&lblProperties[k]);
    }
    
    const size_t clSize = controlList.size();
    
    for(size_t j = 0; j < clSize; ++j) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             controlList[j]->GetWindowTextW().c_str()), pageWidth);
    }

    pageWidth += CS.XGROUPBOX_MARGIN * 2;
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::insertData(GameObject::Builder& builder) {

    // Figure out if the object is in a fixed location or is money, and then
    // handle those special cases, otherwise, just find out what flags need
    // to be set.

    uint8_t newFlags = 0;

    if(btnFlags[6].IsWindowEnabled() &&
       btnFlags[6].GetCheck() == BST_CHECKED) {

        newFlags = GameObjectFlags1::FixedLocation;

    }
    else if(btnFlags[7].IsWindowEnabled() &&
            btnFlags[7].GetCheck() == BST_CHECKED) {

        newFlags = GameObjectFlags1::Money;

        if(btnFlags[1].GetCheck() == BST_CHECKED) {
            newFlags |= GameObjectFlags1::Invisible;
        }

    }
    else {

        for(int i = 0; i < 6; ++i) {
            if(btnFlags[i].GetCheck() == BST_CHECKED) {
                newFlags += (1<<i);
            }
        }

    }

    builder.flags1(newFlags);
    builder.monetaryWorth(spnProperties[0].GetPos());
    builder.uses(spnProperties[1].GetPos());

    const int currentID = builder.getID();
    size_t usedWithIndex = cbxUsedWith.GetCurSel();
    

    if(usedWithIndex == 0) {
        // Nothing selected
        builder.usedWithID(0);
    }
    else if(currentID == GameObjectConstants::NoID) {
        // Selected something, but it's also a new object
        builder.usedWithID(gameMap->objectIDFromIndex(usedWithIndex - 1));
    }
    else {
        // Find the proper Index

        size_t currentIndex = gameMap->objectIndexFromID(currentID);

        if(currentIndex < usedWithIndex) {
            usedWithIndex++;
        }

        builder.usedWithID(gameMap->objectIDFromIndex(usedWithIndex - 1));

    }

}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::moveControls(const WindowMetrics& windowMetrics) {
    
    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();
    
    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultCheckboxSize(maxRowWidth, CD.YCHECKBOX);
    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultTextSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
        CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    // First deal with all the Flags

    for(int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        btnFlags[i].MoveWindow(cPos.x, cPos.y, 
                               defaultCheckboxSize.cx, defaultCheckboxSize.cy);

        if(i == GameObjectFlags1::NumFlags - 1) {
            cPos.Offset(0, defaultCheckboxSize.cy);
        }
        else {
            cPos.Offset(0, defaultCheckboxSize.cy + CS.YRELATED_MARGIN);
        }
    }

    grpFlags.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                        maxGroupBoxWidth, cPos.y);

    // Now deal with things like money and uses

    cPos.Offset(0, CS.YUNRELATED_MARGIN + CS.YFIRST_GROUPBOX_MARGIN);
    
    for(int i = 0; i < 2; i++) {
        lblProperties[i].MoveWindow(cPos.x, cPos.y, 
                                    defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtProperties[i].MoveWindow(cPos.x, cPos.y,
                                    defaultTextSize.cx, defaultTextSize.cy);

        spnProperties[i].SetBuddy(txtProperties[i]);
        cPos.Offset(0, defaultTextSize.cy + CS.YRELATED_MARGIN);
    }

    lblProperties[2].MoveWindow(cPos.x, cPos.y,
                                defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

    cbxUsedWith.MoveWindow(cPos.x, cPos.y, defaultTextSize.cx,
                           CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, CD.YDROPDOWN);

    grpProperties.MoveWindow(CS.XWINDOW_MARGIN,
                             grpFlags.GetClientRect().Height() + CS.YUNRELATED_MARGIN,
                             maxGroupBoxWidth, cPos.y - grpFlags.GetClientRect().Height());


    pageHeight = cPos.y + CS.YUNRELATED_MARGIN;

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::populateFields(const GameObject &gameObject, const GameMap& gameMap) {

    const uint8_t flags = gameObject.getFlags1();
    
    for(int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        if(flags & (1<<i)) {
            btnFlags[i].SetCheck(BST_CHECKED);
            if((flags & (1<<i)) == GameObjectFlags1::Money ||
               (flags & (1<<i)) == GameObjectFlags1::FixedLocation) {
                flagsChanged(ControlIDs::MasterKey+i, BN_CLICKED);
            }
        }
    }

    CString windowText = AtoW(std::to_string(
                              gameObject.getMonetaryWorth()).c_str());

    txtProperties[0].SetWindowText(windowText);

    windowText = AtoW(std::to_string(gameObject.getUses()).c_str());
    txtProperties[1].SetWindowText(windowText);

    size_t usedWithIndex = gameMap.objectIndexFromID(gameObject.getUsedWithID());
    const size_t thisIndex = gameMap.objectIndexFromID(gameObject.getID());

    // Remove the current object from the list.

    if(gameObject.getID() != GameObjectConstants::NoID) {
        cbxUsedWith.DeleteString(thisIndex+1);
    }

    if(usedWithIndex == ((size_t)-1)) {
        cbxUsedWith.SetCurSel(0);
    }
    else {

        if(thisIndex > usedWithIndex) {
            usedWithIndex++;
        }

        cbxUsedWith.SetCurSel(usedWithIndex);
    }
}

//============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// flagsChanged - Updates the check boxes when one of the flags is selected,
/// as well as toggling the two textboxes if necessary.
/// @param an integer containing the control ID of the control being processed
/// @param an integer indicating the notification code the control sent
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::flagsChanged(const WORD& ctrlID, const WORD& ctrlAction) {

    const int which = ctrlID - ControlIDs::MasterKey;
    const bool isChecked = btnFlags[which].GetCheck() == BST_CHECKED ? TRUE : FALSE; 

    // If an object is at a fixed location, it can't have any other flag
    // set. You also can't sell it.
    // TODO: It also cannot be in a character's inventory.

    // If an object is Money, the only other thing it can be is invisible, it
    // also cannot be used, but does have monetary value, and it cannot be used
    // with another object.

    if(ctrlID == ControlIDs::Fixed) {

        for(int i = 0; i < 8; ++i) {
            if(i != which) {
                btnFlags[i].EnableWindow(!isChecked);
            }
        }

        txtProperties[0].EnableWindow(!isChecked);
        
    }
    else if(ctrlID == ControlIDs::Money) {

        for(int i = 0; i < 8; ++i) {
            if(i != which &&
               i != ControlIDs::Invisible - ControlIDs::MasterKey) {

                btnFlags[i].EnableWindow(!isChecked);

            }
        }

        txtProperties[1].EnableWindow(!isChecked);
        cbxUsedWith.EnableWindow(!isChecked);

    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditObjectQualitiesTab::validateFields() {

    // if the object is not in a fixed position, we need to validate it's worth
    if(btnFlags[6].GetCheck() != BST_CHECKED) {
    
        if(!moneyValidator.validate()) {
            return &moneyValidator;
        }

    }

    // If the object is not money, we need to validate the number of uses
    if(btnFlags[7].GetCheck() != BST_CHECKED) {
        if(!usesValidator.validate()) {
            return &usesValidator;
        }
    }

    return NULL;

}

///----------------------------------------------------------------------------
/// updatePropertyValue - Caps the uses and monetary worth text boxes.
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::updatePropertyValue(const WORD& ctrlID) {

    const int ctrlIndex = ctrlID - ControlIDs::UsesBox;

    int newValue = std::stoi(WtoA(txtProperties[ctrlIndex].GetWindowText()).c_str());
    spnProperties[ctrlIndex].SetPos(newValue);

}
