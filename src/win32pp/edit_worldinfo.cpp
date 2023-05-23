#include "edit_worldinfo.h"
#include "../util/languagemapper.h"
#include "shared_functions.h"

namespace ControlIDs {
    const WORD WorldName           = 101;
    const WORD CurrencyName        = 102;
    const WORD txtEnergy           = 103;
    const WORD txtSkill            = 104;
    const WORD txtWillpower        = 105;
    const WORD txtLuck             = 106;
    const WORD txtTorchlife        = 107;
    const WORD spnEnergy           = 108;
    const WORD spnSkill            = 109;
    const WORD spnWillpower        = 110;
    const WORD spnLuck             = 111;
    const WORD spnTorchlife        = 112;
}

//=============================================================================
// Constructors
//=============================================================================
EditWorldInfoDialog::EditWorldInfoDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle) : EditDialogBase(inMainWindow, inGameMap, inParentHandle),
optionChosen(IDCLOSE) {
}


//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditWorldInfoDialog::OnClose() {

    
    const bool wasCanceled = optionChosen != IDOK ? true : false;   

    if(optionChosen == IDCLOSE) {
        // TODO: If changes have been made, prompt the user to ensure they
        // did not accidentally close the window.
    }

    if(optionChosen == IDOK) {
        // Insert Data
    }

    ::EnableWindow(parentWindow, TRUE);
    CWnd::OnClose();
    mainWindow->finishedEditWorldInfoDialog(wasCanceled);

}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditWorldInfoDialog::OnCommand(WPARAM wParam, LPARAM lParam) {
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
    EOD_SetWindowText(LanguageConstants::WorldSettingsGroup, grpWorldInfo, caption, langMap);

    for(int i = 0; i < 2; ++i) {
        lblProperties[i].Create(*this, 0, SS_SIMPLE);
        EOD_SetWindowText(LanguageConstants::WorldNameLabel+i, lblProperties[i], caption, langMap);
        txtProperties[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
        txtProperties[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtProperties[i].LimitText(128);
        txtProperties[i].SetDlgCtrlID(ControlIDs::WorldName+i);
    }

    for(int i = 0; i < 5; ++i) {
        lblAttributes[i].Create(*this, 0, SS_SIMPLE);
        EOD_SetWindowText(LanguageConstants::PlayerEnergy+i, lblAttributes[i], caption, langMap);
        txtAttributes[i].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        spnAttributes[i].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

        txtAttributes[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtAttributes[i].LimitText(2);

        spnAttributes[i].SetRange(GameObjectConstants::MinAttributeValue,
                                  GameObjectConstants::MaxAttributeValue);

        txtAttributes[i].SetDlgCtrlID(ControlIDs::txtEnergy+i);
        spnAttributes[i].SetDlgCtrlID(ControlIDs::spnEnergy+i);
    }

    for(int i = 0; i < 3; ++i) {
        btnDialog[i].Create(*this, 0, BS_PUSHBUTTON);
        EOD_SetWindowText(LanguageConstants::GenericOKButtonCaption+i, btnDialog[i], caption, langMap);
    }

    btnDialog[0].SetStyle(btnDialog[0].GetStyle() | BS_DEFPUSHBUTTON);
    btnDialog[0].SetDlgCtrlID(IDOK);
    btnDialog[1].SetDlgCtrlID(IDCANCEL);

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    calculatePageWidth();

    // TODO: Finish calculating dimensions
    RECT rc = {0, 0,
               pageWidth + (windowMetrics.GetControlSpacing().XWINDOW_MARGIN * 2),
               0};

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

    moveControls();

    return CWnd::OnCreate(cs);
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditWorldInfoDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditWorldInfoDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

void EditWorldInfoDialog::calculatePageWidth() {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for(int i = 0; i < 2; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             lblProperties[i].GetWindowTextW().c_str()), pageWidth);
    }

    for(int i = 0; i < 5; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             lblAttributes[i].GetWindowTextW().c_str()), pageWidth);
    }

    pageWidth += CS.XGROUPBOX_MARGIN * 2;
    pageWidth = std::max(pageWidth, static_cast<LONG>((CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3)));

}

//=============================================================================
// Private Functions
//=============================================================================

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

    for(int i = 0; i < 5; ++i) {

        lblAttributes[i].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtAttributes[i].MoveWindow(cPos.x, cPos.y,
                                    defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

        spnAttributes[i].SetBuddy(txtAttributes[i].GetHwnd());
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
