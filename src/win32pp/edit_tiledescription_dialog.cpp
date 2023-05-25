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

    ::EnableWindow(parentWindow, TRUE);
    CWnd::OnClose();
    //mainWindow->finishedEditWorldInfoDialog(wasCanceled);

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
    lblTileName.SetWindowText(L"Tile Name");

    txtTileName.Create(*this, 0, ES_AUTOHSCROLL);
    txtTileName.SetExStyle(txtTileName.GetExStyle() | WS_EX_CLIENTEDGE);

    lblTileDescription.Create(*this, 0, SS_SIMPLE);
    lblTileDescription.SetWindowText(L"Tile Description");

    txtTileDescription.Create(*this, 0, ES_MULTILINE | WS_VSCROLL);
    txtTileDescription.SetExStyle(txtTileDescription.GetExStyle() | WS_EX_CLIENTEDGE);

    for(int i = 0; i < 2; ++i) {

        btnDialogButtons[i].Create(*this, 0, BS_PUSHBUTTON);
        EOD_SetWindowText(LanguageConstants::GenericOKButtonCaption + 1, btnDialogButtons[i], caption, langMap);

    }

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    moveControls();

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

//=============================================================================
// Private Functions
//=============================================================================

void EditTileDescriptionDialog::moveControls() {
}

