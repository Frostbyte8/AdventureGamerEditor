#include "editstory_dialog.h"
#include "../util/languagemapper.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditStoryDialog::EditStoryDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
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

void EditStoryDialog::OnClose() {

    
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
    mainWindow->finishedEditStoryDialog(wasCanceled);

}
///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditStoryDialog::OnCreate(CREATESTRUCT& cs) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    txtStory.Create(*this, 0, ES_MULTILINE | WS_VSCROLL);
    txtStory.SetExStyle(WS_EX_CLIENTEDGE);
    txtStory.LimitText(8192); // TODO: Figure out a reasonable limit.

    for(int i = 0; i < 2; ++i) {
        dialogButtons[i].Create(*this, 0, BS_PUSHBUTTON);
        EOD_SetWindowText(LanguageConstants::GenericOKButtonCaption+i, dialogButtons[i], caption, langMap);
    }

    dialogButtons[0].SetStyle(dialogButtons[0].GetStyle() | BS_DEFPUSHBUTTON);
    
    return CWnd::OnCreate(cs);
}


///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for the dialog
///----------------------------------------------------------------------------

void EditStoryDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditStoryDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}
