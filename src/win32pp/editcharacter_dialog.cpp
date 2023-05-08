#include "editcharacter_dialog.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditCharacterDialog(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics,
                    const GameMap* inGameMap, bool inEditCharacter) : mainWindow(inMainWindow),
                    windowMetrics(inWindowMetrics), gameMap(inGameMap) {
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterDialog::OnCreate(CREATESTRUCT& cs) {
    return CWnd::OnCreate(cs);
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the Frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT EditCharacterDialog::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
    return WndProcDefault(msg, wParam, lParam);
}
