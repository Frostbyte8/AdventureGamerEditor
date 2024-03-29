#include "edit_worldinfo.h"
#include "../../util/languagemapper.h"
#include "../shared_functions.h"
#include "../../util/win32pp_extras.h"
#include "../../adventuregamer_constants.h"

namespace ControlIDs {
    const WORD WorldName            = 101;
    const WORD CurrencyName         = 102;
    const WORD txtEnergy            = 103;
    const WORD txtEnergyRandom      = 104;
    const WORD txtSkill             = 105;
    const WORD txtSkillRandom       = 106;
    const WORD txtWillpower         = 107;
    const WORD txtWillpowerRandom   = 108;
    const WORD txtLuck              = 109;
    const WORD txtLuckRandom        = 110;
    const WORD txtTorchlife         = 111;
    const WORD txtTorchlifeRandom   = 112;
    const WORD spnEnergy            = 113;
    const WORD spnSkill             = 114;
    const WORD spnWillpower         = 115;
    const WORD spnLuck              = 116;
    const WORD spnTorchlife         = 117;
}

//=============================================================================
// Constructors
//=============================================================================

EditWorldInfoDialog::EditWorldInfoDialog(MainWindowInterface* inMainWindow, HWND inParentHandle) : 
EditDialogBase(inMainWindow, inParentHandle, &MainWindowInterface::finishedEditWorldInfoDialog) {
}

//=============================================================================
// Accessors
//=============================================================================

GameInfo EditWorldInfoDialog::getGameInfo() {
    return newGameInfo;
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setWorldInfo - Give the dialog a GameInfo object to populate its fields
/// with.
/// @param a constant reference containing the GameInfo structure to copy
/// the data from.
///----------------------------------------------------------------------------

void EditWorldInfoDialog::setWorldInfo(const GameInfo& gameInfo) {

    CString caption;

    SetWindowTextFromStr(gameInfo.getGameName(), txtProperties[0], caption);
    SetWindowTextFromStr(gameInfo.getCurrencyName(), txtProperties[1], caption);

    for(int i = 0; i < AttributeTypes::NumTypes; ++i) {
        int base = gameInfo.getBaseAttribute(i);
        int random = gameInfo.getRandomAttribute(i);
        spnAttributes[i].SetPos(base);
        spnRandomAttributes[i].SetPos(random);
    }

    // TODO: this should not be here

    newGameInfo.setPlayerX(gameInfo.getPlayerStartX());
    newGameInfo.setPlayerY(gameInfo.getPlayerStartY());

}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditWorldInfoDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    const WORD ctrlID = LOWORD(wParam);
    const WORD notifyCode = HIWORD(wParam);

    if(lParam) {
        if(ctrlID == IDOK || ctrlID == IDCANCEL || ctrlID == DefControlIDs::IDAPPLY) {
            if(notifyCode == BN_CLICKED) {
                dialogButtonPressed(ctrlID);
                return TRUE;
            }
        }
        else if(ctrlID >= ControlIDs::WorldName && ctrlID <= ControlIDs::txtTorchlifeRandom) {
            if(notifyCode == EN_CHANGE) {
                madeChange();
                return TRUE;
            }
        }
    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditWorldInfoDialog::OnCreate(CREATESTRUCT& cs) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpWorldInfo.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("WorldSettingsGroup", grpWorldInfo, caption, langMap);

    for(int i = 0; i < 2; ++i) {
        lblProperties[i].Create(*this, 0, SS_SIMPLE);
        txtProperties[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
        txtProperties[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtProperties[i].SetDlgCtrlID(ControlIDs::WorldName+i);
    }

    txtProperties[0].SetLimitText(GameInfoConstants::MaxWorldName);
    txtProperties[1].SetLimitText(GameInfoConstants::MaxCurrencyName);

    SetWindowTextFromLangMapString("WorldNameLabel", lblProperties[0], caption, langMap);
    SetWindowTextFromLangMapString("CurrencyNameLabel", lblProperties[1], caption, langMap);

    for(int i = 0; i < AttributeTypes::NumTypes; ++i) {
        lblAttributes[i].Create(*this, 0, SS_SIMPLE);       

        txtAttributes[i].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        spnAttributes[i].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

        txtAttributes[i].SetDlgCtrlID(ControlIDs::txtEnergy + (i * 2));

        txtRandomAttributes[i].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        spnRandomAttributes[i].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                      UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

        txtRandomAttributes[i].SetDlgCtrlID(ControlIDs::txtEnergyRandom + (i * 2));

        txtAttributes[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtAttributes[i].LimitText(2);
        txtRandomAttributes[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtRandomAttributes[i].LimitText(2);

        spnAttributes[i].SetRange(AdventureGamerConstants::MinAttributeValue,
                                  AdventureGamerConstants::MaxAttributeValue);

        spnRandomAttributes[i].SetRange(AdventureGamerConstants::MinAttributeValue,
                                        AdventureGamerConstants::MaxAttributeValue);

        txtAttributes[i].SetDlgCtrlID(ControlIDs::txtEnergy+i);
        spnAttributes[i].SetDlgCtrlID(ControlIDs::spnEnergy+i);

        baseAttributeValidator[i] = IntegerValidator(&txtAttributes[i], AdventureGamerConstants::MinAttributeValue,
                                                     AdventureGamerConstants::MaxAttributeValue);

        randomAttributeValidator[i] = IntegerValidator(&txtRandomAttributes[i], AdventureGamerConstants::MinAttributeValue,
                                                       AdventureGamerConstants::MaxAttributeValue);
    }

    SetWindowTextFromLangMapString("EnergyLabel", lblAttributes[0], caption, langMap);
    SetWindowTextFromLangMapString("SkillLabel", lblAttributes[1], caption, langMap);
    SetWindowTextFromLangMapString("WillpowerLabel", lblAttributes[2], caption, langMap);
    SetWindowTextFromLangMapString("LuckLabel", lblAttributes[3], caption, langMap);
    SetWindowTextFromLangMapString("TorchLifeLabel", lblAttributes[4], caption, langMap);

    createDefaultDialogButtons(true);

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    const LONG contentWidth = calculateWindowWidth();

    // TODO: Finish calculating dimensions
    RECT rc = {0, 0,
               contentWidth + (windowMetrics.GetControlSpacing().XWINDOW_MARGIN * 2),
               0};

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

    moveControls();

    return CWnd::OnCreate(cs);
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for the dialog
///----------------------------------------------------------------------------

void EditWorldInfoDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = _T("EditWorldInfoDialog");
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// moveControls - Move the controls into their proper positions
///----------------------------------------------------------------------------

void EditWorldInfoDialog::moveControls() {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    // The max width of a row is the size of the window, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultEditSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);
    
    for(int i = 0; i < 2; ++i) {

        lblProperties[i].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtProperties[i].MoveWindow(cPos.x, cPos.y,
                                    defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

    }

    for(int i = 0; i < AttributeTypes::NumTypes; ++i) {

        lblAttributes[i].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtAttributes[i].MoveWindow(cPos.x, cPos.y,
                                    defaultEditSize.cx / 2, defaultEditSize.cy);

        txtRandomAttributes[i].MoveWindow(cPos.x + (defaultEditSize.cx / 2), cPos.y,
                                          defaultEditSize.cx / 2, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

        spnAttributes[i].SetBuddy(txtAttributes[i].GetHwnd());
        spnRandomAttributes[i].SetBuddy(txtRandomAttributes[i].GetHwnd());

    }

    grpWorldInfo.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN, maxGroupBoxWidth, cPos.y);

    cPos.Offset(0, CS.YUNRELATED_MARGIN);

    CPoint btnPos(GetClientRect().right - CS.XWINDOW_MARGIN, cPos.y + CS.YUNRELATED_MARGIN);
    btnPos.Offset(-(CD.XBUTTON), 0);

    for(int i = 2; i >= 0; --i) {
        btnDialog[i].MoveWindow(btnPos.x, btnPos.y, CD.XBUTTON, CD.YBUTTON);
        btnPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }

    btnPos.Offset(0, CD.YBUTTON);

    // TODO: Finish calculating dimensions
    RECT rc = {0, 0,
               maxGroupBoxWidth + (CS.XWINDOW_MARGIN * 2),
               btnPos.y + (CS.YWINDOW_MARGIN * 2) };

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

}

///----------------------------------------------------------------------------
/// trySaveData - Confirm data is valid, and if it is save it. This function
/// should not be called directly and instead trySave should be called instead.
/// @return true if the data was valid, false if it was not.
///----------------------------------------------------------------------------

bool EditWorldInfoDialog::trySaveData() {
    
    const InputValidator* validationFailed = validateFields();
    
    if(validationFailed) {
    
        std::string errorMessage;
        std::string errorTitle;

        processValidatorError(errorMessage, errorTitle, validationFailed);
        displayErrorMessage(errorMessage, errorTitle);

        // TODO: Return an ID instead of a HWND
        // Also, tell the tab what action to take (IE: if it's a textbox,
        // hilight it)
        if(validationFailed->getErrorCode() != errorCodes::ControlNotFound) {
            validationFailed->getWindow()->SetFocus();
        }
    
        return false;
    }
    
    newGameInfo.setGameName(TtoA(txtProperties[0].GetWindowText()).c_str());
    newGameInfo.setCurrencyName(TtoA(txtProperties[1].GetWindowText()).c_str());
    
    for(int i = 0; i < AttributeTypes::NumTypes; ++i) {
        newGameInfo.setBaseAttribute(i, std::stoi(TtoA(txtAttributes[i].GetWindowText()).c_str()));
        newGameInfo.setRandomAttribute(i, std::stoi(TtoA(txtRandomAttributes[i].GetWindowText()).c_str()));
    }
       
    return true;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// calculateWindowWidth - Calculates how wide the content of the window
/// needs to be.
/// @return a LONG integer containing the width of the content.
///----------------------------------------------------------------------------

LONG EditWorldInfoDialog::calculateWindowWidth() {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    LONG contentWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for(int i = 0; i < 2; ++i) {
        contentWidth = std::max(windowMetrics.CalculateStringWidth(
                                lblProperties[i].GetWindowText().c_str()), contentWidth);
    }

    for(int i = 0; i < AttributeTypes::NumTypes; ++i) {
        contentWidth = std::max(windowMetrics.CalculateStringWidth(
                                lblAttributes[i].GetWindowText().c_str()), contentWidth);
    }

    contentWidth += CS.XGROUPBOX_MARGIN * 2;
    contentWidth = std::max(contentWidth, static_cast<LONG>((CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3)));
    
    return contentWidth;
}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditWorldInfoDialog::validateFields() {
    
    for(int i = 0; i < AttributeTypes::NumTypes; ++i) {
    
        if(!baseAttributeValidator[i].validate()) {
            return &baseAttributeValidator[i];
        }

        if(!randomAttributeValidator[i].validate()) {
            return &baseAttributeValidator[i];
        }
    
    }
    
    return NULL;
    
}