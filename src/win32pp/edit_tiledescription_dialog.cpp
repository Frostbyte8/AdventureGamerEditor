#include "edit_tiledescription_dialog.h"
#include "../util/languagemapper.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditTileDescriptionDialog::EditTileDescriptionDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
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

void EditTileDescriptionDialog::OnClose() {
    const bool wasCanceled = optionChosen != IDOK ? true : false;   
    ::EnableWindow(parentWindow, TRUE);
    CWnd::OnClose();
    mainWindow->finishedEditTileDescriptionDialog(wasCanceled, false);

}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditTileDescriptionDialog::OnCommand(WPARAM wParam, LPARAM lParam) {
    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditTileDescriptionDialog::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    lblTileName.Create(*this, 0, SS_SIMPLE);
    EOD_SetWindowText(LanguageConstants::TileNameLabel, lblTileName, caption, langMap);

    txtTileName.Create(*this, 0, ES_AUTOHSCROLL);
    txtTileName.SetExStyle(txtTileName.GetExStyle() | WS_EX_CLIENTEDGE);

    lblTileDescription.Create(*this, 0, SS_SIMPLE);
    EOD_SetWindowText(LanguageConstants::TileDescriptionLabel, lblTileDescription, caption, langMap);

    txtTileDescription.Create(*this, 0, ES_MULTILINE | WS_VSCROLL);
    txtTileDescription.SetExStyle(txtTileDescription.GetExStyle() | WS_EX_CLIENTEDGE);

    for(int i = 0; i < 3; ++i) {

        btnDialogButtons[i].Create(*this, 0, BS_PUSHBUTTON);
        EOD_SetWindowText(LanguageConstants::GenericOKButtonCaption + i, btnDialogButtons[i], caption, langMap);

    }

    btnDialogButtons[0].SetStyle(btnDialogButtons[0].GetStyle() | BS_DEFPUSHBUTTON);

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    moveControls();

    dialogCaption = LM_toUTF8(LanguageConstants::EditTileDescrtipionDialogCaption, langMap);

    SetWindowText(dialogCaption);

    return retVal;
}


///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for the dialog
///----------------------------------------------------------------------------

void EditTileDescriptionDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditTileDescriptionDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}


void EditTileDescriptionDialog::setTileDescription(const std::string& inName, const std::string& inDescription) {

    if(txtTileName.IsWindow()) {
        txtTileName.SetWindowText(AtoW(inName.c_str()));
        txtTileDescription.SetWindowText(AtoW(inDescription.c_str()));
    }

}

//=============================================================================
// Private Functions
//=============================================================================

void EditTileDescriptionDialog::moveControls() {

    // TODO: At some point, make this dialog resizable, also figure out actual width

    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();
    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();

    const int minWidth = (CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3) + (CS.XWINDOW_MARGIN * 2);
    const int innerWidth = minWidth - (CS.XWINDOW_MARGIN * 2);

    CPoint cPos(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN);

    lblTileName.MoveWindow(cPos.x, cPos.y, innerWidth, CD.YLABEL);
    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);
    
    txtTileName.MoveWindow(cPos.x, cPos.y, innerWidth, CD.YTEXTBOX_ONE_LINE_ALONE);
    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN);

    lblTileDescription.MoveWindow(cPos.x, cPos.y, innerWidth, CD.YLABEL);
    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);

    txtTileDescription.MoveWindow(cPos.x, cPos.y, innerWidth, (CD.YTEXTBOX_ONE_LINE_ALONE) + (CD.YTEXTBOX_ADDITIONAL_LINES * 5));
    cPos.Offset(innerWidth - (CD.XBUTTON),
                txtTileDescription.GetClientRect().Height() + CS.YUNRELATED_MARGIN);

    for(int i = 2; i >= 0; --i) {
        btnDialogButtons[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        cPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }

    // TODO: Finish calculating dimensions
    RECT rc = {0, 0, minWidth, cPos.y + CS.YWINDOW_MARGIN + CD.YBUTTON};

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

}
