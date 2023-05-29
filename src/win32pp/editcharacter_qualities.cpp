#include "editcharacter_tabviews.h"
#include "../model/gamecharacter.h"

#include "shared_functions.h"

namespace ControlIDs {
    const WORD MoneyOnHand      = 101;
    const WORD CharType         = 102;
    const WORD FlagEnterDark    = 103;
    const WORD FlagEnterHazard  = 104;
    const WORD FlagCanJump      = 105;
    const WORD FlagBypassGates  = 106;
    const WORD FlagBypassDoors  = 107;
    const WORD FlagCanClimb     = 108;
    const WORD FlagWanderer     = 109;
    const WORD FlagStalker      = 110;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Process the WM_COMMAND message. Refer to to the Win32++
/// documentation for more details.
///----------------------------------------------------------------------------

BOOL EditCharacterQualitiesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(!lParam) {
        return FALSE;
    }

    const WORD ctrlID = LOWORD(wParam);
    const WORD ctrlAction = HIWORD(wParam);

    if(ctrlID == ControlIDs::MoneyOnHand) {
        if(ctrlAction == EN_KILLFOCUS) {
            const int newValue = std::stoi(WtoA(txtMoney.GetWindowText()).c_str());
            spnMoney.SetPos(newValue);
        }
        else if(ctrlAction == EN_CHANGE) {
            parentWindow->madeChange();
        }
    }
    else if(ctrlID >= ControlIDs::FlagEnterDark && ctrlID <= ControlIDs::FlagStalker) {
        if(ctrlAction == BN_CLICKED) {
            parentWindow->madeChange();
        }
    }
    else if(ctrlID == ControlIDs::CharType) {
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
/// OnCreate - Set some defaults for the tab, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterQualitiesTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);
    
    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpFlags.Create(*this, 0, BS_GROUPBOX);

    EOD_SetWindowText("FileMenu", grpFlags,
                      caption, langMap);

    for(int k = 0; k < GameCharacterFlags::NumFlags; ++k) {

        btnFlags[k].Create(*this, 0, BS_AUTOCHECKBOX | WS_TABSTOP);
        EOD_SetWindowText("FileMenu", btnFlags[k],
                          caption, langMap);

        btnFlags[k].SetDlgCtrlID(ControlIDs::FlagEnterDark+k);

    }

    grpProperties.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText("FileMenu", grpProperties, caption, langMap);

    lblMoney.Create(*this, 0, SS_SIMPLE);
    EOD_SetWindowText("FileMenu", lblMoney, caption, langMap);

    txtMoney.Create(*this, 0, ES_AUTOHSCROLL);
    txtMoney.SetExStyle(WS_EX_CLIENTEDGE);
    txtMoney.LimitText(5);
    txtMoney.SetDlgCtrlID(ControlIDs::MoneyOnHand);
    spnMoney.Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY | UDS_NOTHOUSANDS |
                    UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

    lblType.Create(*this, 0, SS_SIMPLE);
    EOD_SetWindowText("FileMenu", lblType, caption, langMap);
    cbxType.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL | WS_TABSTOP);
    cbxType.SetDlgCtrlID(ControlIDs::CharType);

    for(size_t j = 0; j < 3; ++j) {
        EOD_AddString("FileMenu", cbxType, caption, langMap);
    }

    spnMoney.SetRange(GameObjectConstants::MinMonetaryValue,
                      GameObjectConstants::MaxMonetaryValue);
                      
    // TODO: These Money constants should be moved to "GameWorldConstants" namespace
    moneyValidator = IntegerValidator(&txtMoney, GameObjectConstants::MinMonetaryValue, GameObjectConstants::MaxMonetaryValue);

    return retVal;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterQualitiesTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::calculatePageWidth(const WindowMetrics& windowMetrics) {
    
    pageWidth = 0;

    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();

    const LONG checkboxWidth = CD.XCHECKBOX + CD.XCHECKBOX_GAP;
    
    for(int i = 0; i < GameCharacterFlags::NumFlags; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             btnFlags[i].GetWindowTextW().c_str()) + checkboxWidth,
                             pageWidth);
    }

    pageWidth = std::max(windowMetrics.CalculateStringWidth(
                         lblMoney.GetWindowTextW().c_str()), pageWidth);

    pageWidth = std::max(windowMetrics.CalculateStringWidth(
                         lblType.GetWindowTextW().c_str()), pageWidth);

}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::insertData(GameCharacter::Builder& builder) {

    uint8_t newFlags = 0;

    for(int i = 0; i < 6; ++i) {
        if(btnFlags[i].GetCheck() == BST_CHECKED) {
            newFlags += (1<<i);
        }
    }

    builder.flags(newFlags);
    builder.money(spnMoney.GetPos());
    builder.type(cbxType.GetCurSel());

}


///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::moveControls(const WindowMetrics& windowMetrics) {
    
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

    for(int i = 0; i < GameCharacterFlags::NumFlags; ++i) {
        btnFlags[i].MoveWindow(cPos.x, cPos.y, 
                               defaultCheckboxSize.cx, defaultCheckboxSize.cy);

        if(i == GameCharacterFlags::NumFlags - 1) {
            cPos.Offset(0, defaultCheckboxSize.cy);
        }
        else {
            cPos.Offset(0, defaultCheckboxSize.cy + CS.YRELATED_MARGIN);
        }
    }

    grpFlags.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                        maxGroupBoxWidth, cPos.y);

    
    // Now deal with money, and the character type

    cPos.Offset(0, CS.YUNRELATED_MARGIN + CS.YFIRST_GROUPBOX_MARGIN);
    
    lblMoney.MoveWindow(cPos.x, cPos.y, 
                        defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

    txtMoney.MoveWindow(cPos.x, cPos.y,
                        defaultTextSize.cx, defaultTextSize.cy);

    spnMoney.SetBuddy(txtMoney);
    cPos.Offset(0, defaultTextSize.cy + CS.YRELATED_MARGIN);

    lblType.MoveWindow(cPos.x, cPos.y,
                       defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

    cbxType.MoveWindow(cPos.x, cPos.y, defaultTextSize.cx,
                           CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, CD.YDROPDOWN);

    grpProperties.MoveWindow(CS.XWINDOW_MARGIN,
                             grpFlags.GetClientRect().Height() + CS.YUNRELATED_MARGIN,
                             maxGroupBoxWidth, cPos.y - grpFlags.GetClientRect().Height());

    pageHeight = cPos.y + CS.YUNRELATED_MARGIN;

}

///----------------------------------------------------------------------------
/// populateFields - 
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::populateFields(const GameCharacter& gameCharacter) {

    const uint8_t flags = gameCharacter.getFlags();

    for(int i = 0; i < GameCharacterFlags::NumFlags; ++i) {
        if(flags & (1<<i)) {
            btnFlags[i].SetCheck(BST_CHECKED);
        }
    }

    spnMoney.SetPos(gameCharacter.getMoney());
    cbxType.SetCurSel(gameCharacter.getType());

}
 
///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterQualitiesTab::validateFields() {

    if(!moneyValidator.validate()) {
        return &moneyValidator;
    }

    return NULL;

}