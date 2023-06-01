#include "edit_worldinfo.h"
#include "../util/languagemapper.h"
#include "shared_functions.h"
#include "../adventuregamer_constants.h"


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
EditDialogBase(inMainWindow, inParentHandle) {
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
///----------------------------------------------------------------------------

void EditWorldInfoDialog::setWorldInfo(const GameInfo& gameInfo) {

    CString caption;

    EOD_SetWindowText(gameInfo.getGameName(), txtProperties[0], caption);
    EOD_SetWindowText(gameInfo.getCurrencyName(), txtProperties[1], caption);

    for(int i = 0; i < AttributeTypes::NumTypes; ++i) {
        int base = gameInfo.getBaseAttribute(i);
        int random = gameInfo.getRandomAttribute(i);
        spnAttributes[i].SetPos(base);
        spnRandomAttributes[i].SetPos(random);
    }

}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditWorldInfoDialog::OnClose() {
    
    // First we'll see if we can actually close the dialog.
    if(!tryClose()) {
        return;
    }

    // Then we'll end the dialog and inform the parent window
    // that we are done.
    
    endModal(&MainWindowInterface::finishedEditWorldInfoDialog);

}

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
    EOD_SetWindowText("WorldSettingsGroup", grpWorldInfo, caption, langMap);

    for(int i = 0; i < 2; ++i) {
        lblProperties[i].Create(*this, 0, SS_SIMPLE);
        txtProperties[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
        txtProperties[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtProperties[i].SetDlgCtrlID(ControlIDs::WorldName+i);
    }

    txtProperties[0].SetLimitText(GameInfoConstants::MaxWorldName);
    txtProperties[1].SetLimitText(GameInfoConstants::MaxCurrencyName);

    EOD_SetWindowText("WorldNameLabel", lblProperties[0], caption, langMap);
    EOD_SetWindowText("CurrencyNameLabel", lblProperties[1], caption, langMap);

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

    }

    EOD_SetWindowText("EnergyLabel", lblAttributes[0], caption, langMap);
    EOD_SetWindowText("SkillLabel", lblAttributes[1], caption, langMap);
    EOD_SetWindowText("WillpowerLabel", lblAttributes[2], caption, langMap);
    EOD_SetWindowText("LuckLabel", lblAttributes[3], caption, langMap);
    EOD_SetWindowText("TorchLifeLabel", lblAttributes[4], caption, langMap);

    for(int i = 0; i < 3; ++i) {
        btnDialog[i].Create(*this, 0, BS_PUSHBUTTON);
    }

    EOD_SetWindowText("OKButton", btnDialog[0], caption, langMap);
    EOD_SetWindowText("CancelButton", btnDialog[1], caption, langMap);
    EOD_SetWindowText("ApplyButton", btnDialog[2], caption, langMap);

    btnDialog[0].SetStyle(btnDialog[0].GetStyle() | BS_DEFPUSHBUTTON);
    btnDialog[0].SetDlgCtrlID(IDOK);
    btnDialog[1].SetDlgCtrlID(IDCANCEL);
    btnDialog[2].SetDlgCtrlID(DefControlIDs::IDAPPLY);
    btnDialog[2].EnableWindow(FALSE);

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    const LONG contentWidth = caclculateWindowWidth();

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
    wc.lpszClassName = L"EditWorldInfoDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// notifyChangeMade - Change the apply button to be useable.
///----------------------------------------------------------------------------

void EditWorldInfoDialog::notifyChangeMade() {
    btnDialog[2].EnableWindow(TRUE);
}

///----------------------------------------------------------------------------
/// notifyChangesSaved - Change the apply button to be unusable.
///----------------------------------------------------------------------------

void EditWorldInfoDialog::notifyChangesSaved() {
    btnDialog[2].EnableWindow(FALSE);
}

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
    // TODO: Write function
    return true;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// caclculateCotentWidth - Calculates how wide the content of the window
/// needs to be. TODO: Should this add the Window margin as well?
/// @return a LONG integer containing the width of the content.
///----------------------------------------------------------------------------

LONG EditWorldInfoDialog::caclculateWindowWidth() {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    LONG contentWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for(int i = 0; i < 2; ++i) {
        contentWidth = std::max(windowMetrics.CalculateStringWidth(
                                lblProperties[i].GetWindowTextW().c_str()), contentWidth);
    }

    for(int i = 0; i < AttributeTypes::NumTypes; ++i) {
        contentWidth = std::max(windowMetrics.CalculateStringWidth(
                                lblAttributes[i].GetWindowTextW().c_str()), contentWidth);
    }

    contentWidth += CS.XGROUPBOX_MARGIN * 2;
    contentWidth = std::max(contentWidth, static_cast<LONG>((CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3)));
    
    return contentWidth;
}
