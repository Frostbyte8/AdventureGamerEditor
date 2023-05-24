#include "editstory_dialog.h"
#include "../util/languagemapper.h"
#include "shared_functions.h"

namespace ControlIDs {
    const WORD ID_APPLY = 101;
}

//=============================================================================
// Constructors
//=============================================================================

EditStoryDialog::EditStoryDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle) : EditDialogBase(inMainWindow, inGameMap, inParentHandle),
optionChosen(IDCLOSE), changeMade(false) {
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditStoryDialog::OnClose() {

    if(optionChosen == IDCLOSE) {

        if(changeMade) {

            LanguageMapper& langMap = LanguageMapper::getInstance();

            CString message = LM_toUTF8(LanguageConstants::UnsavedChangesMessage, langMap);
            CString title = LM_toUTF8(LanguageConstants::UnsavedChangesTitle, langMap);

            const int response = MessageBox(message, title, MB_ICONQUESTION | MB_YESNOCANCEL);

            if(response == IDYES) {
                okClicked();
                optionChosen = IDOK;
            }
            else if(response == IDNO) {
                optionChosen = IDCANCEL;
            }
            else {
                return; // Don't do anything.
            }
        }
        else {
            optionChosen = IDCANCEL;
        }
    }

    const bool wasCanceled = optionChosen != IDOK ? true : false;   

    if(optionChosen == IDOK || optionChosen == IDCANCEL) {
        // If either OK or Canceled are pressed, we need to give control
        // back to the parent window
        ::EnableWindow(parentWindow, TRUE);
        CWnd::OnClose();
        mainWindow->finishedEditStoryDialog(wasCanceled, false);
    }
    else if(optionChosen == ControlIDs::ID_APPLY) {
        // Just apply, don't close.
        mainWindow->finishedEditStoryDialog(false, true);
        optionChosen = IDCLOSE;
        changeMade = false;
    }
}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditStoryDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    const WORD ctrlID = LOWORD(wParam);
    const WORD notifyCode = HIWORD(wParam);

    if(lParam) {
        if(notifyCode == BN_CLICKED) {
            if(ctrlID == IDOK) {
                 
                okClicked();
                optionChosen = IDOK;
                Close();
                return TRUE;

            }
            else if(ctrlID == ControlIDs::ID_APPLY) {
                okClicked();
                optionChosen = ControlIDs::ID_APPLY;
                Close();
                return TRUE;
            }
            else if(ctrlID == IDCANCEL) {
                optionChosen = IDCANCEL;
                Close();
                return TRUE;
            }
        }
        else if(notifyCode == EN_CHANGE) {
            changeMade = true;
        }
    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditStoryDialog::OnCreate(CREATESTRUCT& cs) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    lblSummary.Create(*this, 0, SS_SIMPLE);
    lblSummary.SetWindowText(L"Summary");

    txtSummary.Create(*this, 0, ES_MULTILINE | WS_VSCROLL);
    txtSummary.SetExStyle(WS_EX_CLIENTEDGE);
    txtSummary.LimitText(8192); // TODO: Figure out a reasonable limit.

    lblStory.Create(*this, 0, SS_SIMPLE);
    lblStory.SetWindowText(L"Story");

    txtStory.Create(*this, 0, ES_MULTILINE | WS_VSCROLL);
    txtStory.SetExStyle(WS_EX_CLIENTEDGE);
    txtStory.LimitText(8192); // TODO: Figure out a reasonable limit.

    for(int i = 0; i < 3; ++i) {
        dialogButtons[i].Create(*this, 0, BS_PUSHBUTTON);
        EOD_SetWindowText(LanguageConstants::GenericOKButtonCaption+i, dialogButtons[i], caption, langMap);
    }

    dialogButtons[0].SetStyle(dialogButtons[0].GetStyle() | BS_DEFPUSHBUTTON);

    dialogButtons[0].SetDlgCtrlID(IDOK);
    dialogButtons[1].SetDlgCtrlID(IDCANCEL);
    dialogButtons[2].SetDlgCtrlID(ControlIDs::ID_APPLY);

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    moveControls();
    
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

//=============================================================================
// Public Functions
//=============================================================================

void EditStoryDialog::setStoryAndSummary(const std::string& inStoryText, const std::string& inSummaryText) {
    if(txtStory.IsWindow()) {
        txtStory.SetWindowText(AtoW(inStoryText.c_str()));
        txtSummary.SetWindowText(AtoW(inSummaryText.c_str()));
    }
}

const std::string EditStoryDialog::getStory() {
    return storyText;
}

const std::string EditStoryDialog::getSummary() {
    return summaryText;
}

//=============================================================================
// Private Functions
//=============================================================================

void EditStoryDialog::moveControls() {

    // TODO: At some point, make this dialog resizeable 

    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();
    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();

    const int minWidth = (CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3) + (CS.XWINDOW_MARGIN * 2);
    const int boundryWidth = minWidth - CS.XWINDOW_MARGIN * 2;

    CPoint cPos(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN);

    lblSummary.MoveWindow(cPos.x, cPos.y, boundryWidth, CD.YLABEL);

    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);

    txtSummary.MoveWindow(cPos.x, cPos.y,
                          boundryWidth, CD.YTEXTBOX_ONE_LINE_ALONE + (CD.YTEXTBOX_ADDITIONAL_LINES * 5));

    cPos.Offset(0, txtSummary.GetClientRect().Height() + CS.YRELATED_MARGIN);

    lblStory.MoveWindow(cPos.x, cPos.y, boundryWidth, CD.YLABEL);

    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);

    txtStory.MoveWindow(cPos.x, cPos.y,
                        boundryWidth, CD.YTEXTBOX_ONE_LINE_ALONE + (CD.YTEXTBOX_ADDITIONAL_LINES * 5));

    cPos.Offset(boundryWidth - (CD.XBUTTON), txtSummary.GetClientRect().Height() + CS.YUNRELATED_MARGIN);

    for(int i = 2; i >= 0; --i) {
        dialogButtons[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        cPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }

    // TODO: Finish calculating dimensions
    RECT rc = {0, 0, minWidth, cPos.y + CS.YWINDOW_MARGIN + CD.YBUTTON};

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

}

void EditStoryDialog::okClicked() {
    storyText = WtoA(txtStory.GetWindowText());
    summaryText = WtoA(txtSummary.GetWindowText());
}
