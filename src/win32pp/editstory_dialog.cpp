#include "editstory_dialog.h"
#include "../util/languagemapper.h"
#include "shared_functions.h"

namespace ControlIDs {
    const WORD SummaryText  = 101;
    const WORD StoryText    = 102;
}

//=============================================================================
// Constructors
//=============================================================================

EditStoryDialog::EditStoryDialog(MainWindowInterface* inMainWindow, HWND inParentHandle) : 
EditDialogBase(inMainWindow, inParentHandle), optionChosen(IDCLOSE), dialogCaption("") {
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getStory - Get a copy of the story as it was last saved in the dialog.
/// @return a copy of the story as it was last saved.
///----------------------------------------------------------------------------

const std::string EditStoryDialog::getStory() {
    return storyText;
}

///----------------------------------------------------------------------------
/// getSummary - Get a copy of the world's summary as it was last saved in the
/// dialog.
/// @return a copy of the summary as it was last saved.
///----------------------------------------------------------------------------

const std::string EditStoryDialog::getSummary() {
    return summaryText;
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setStoryAndSummary - Set the story and summary that the Game World has so
/// that it can be altered by the user.
/// @param a string containing the text of the story.
/// @param a string containing the summary of the story
///----------------------------------------------------------------------------

void EditStoryDialog::setStoryAndSummary(const std::string& inStoryText, const std::string& inSummaryText) {
    if(txtStory.IsWindow()) {
        txtStory.SetWindowText(AtoW(inStoryText.c_str()));
        txtSummary.SetWindowText(AtoW(inSummaryText.c_str()));
    }
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditStoryDialog::OnClose() {

    // First we'll see if we can actually close the dialog.
    if(!tryClose()) {
        return;
    }

    // Then we'll end the dialog and inform the parent window
    // that we are done.
    endModal(&MainWindowInterface::finishedEditStoryDialog);
   
}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditStoryDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    const WORD ctrlID = LOWORD(wParam);
    const WORD notifyCode = HIWORD(wParam);

    if(lParam) {
        if(ctrlID == IDOK || ctrlID == IDCANCEL || ctrlID == DefControlIDs::IDAPPLY) {
            if(notifyCode == BN_CLICKED) {
                dialogButtonPressed(ctrlID);
                return TRUE;
            }
        }
        else if(ctrlID == ControlIDs::SummaryText || ctrlID == ControlIDs::StoryText) {
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

int EditStoryDialog::OnCreate(CREATESTRUCT& cs) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    dialogCaption = LM_toUTF8("FileMenu", langMap);
    SetWindowText(dialogCaption);

    lblSummary.Create(*this, 0, SS_SIMPLE);
    lblSummary.SetWindowText(L"Summary");

    txtSummary.Create(*this, 0, ES_MULTILINE | WS_VSCROLL);
    txtSummary.SetExStyle(WS_EX_CLIENTEDGE);
    txtSummary.LimitText(8192); // TODO: Figure out a reasonable limit.
    txtSummary.SetDlgCtrlID(ControlIDs::SummaryText);

    lblStory.Create(*this, 0, SS_SIMPLE);
    lblStory.SetWindowText(L"Story");

    txtStory.Create(*this, 0, ES_MULTILINE | WS_VSCROLL);
    txtStory.SetExStyle(WS_EX_CLIENTEDGE);
    txtStory.LimitText(8192); // TODO: Figure out a reasonable limit.
    txtStory.SetDlgCtrlID(ControlIDs::SummaryText);

    for(int i = 0; i < 3; ++i) {
        dialogButtons[i].Create(*this, 0, BS_PUSHBUTTON);
        EOD_SetWindowText("FileMenu", dialogButtons[i], caption, langMap);
    }

    dialogButtons[0].SetStyle(dialogButtons[0].GetStyle() | BS_DEFPUSHBUTTON);

    dialogButtons[0].SetDlgCtrlID(IDOK);
    dialogButtons[1].SetDlgCtrlID(IDCANCEL);
    dialogButtons[2].SetDlgCtrlID(DefControlIDs::IDAPPLY);
    dialogButtons[2].EnableWindow(FALSE);

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
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// notifyChangeMade - Change the apply button to be useable.
///----------------------------------------------------------------------------

void EditStoryDialog::notifyChangeMade() {
    dialogButtons[2].EnableWindow(TRUE);
}

///----------------------------------------------------------------------------
/// notifyChangesSaved - Change the apply button to be unusable.
///----------------------------------------------------------------------------

void EditStoryDialog::notifyChangesSaved() {
    dialogButtons[2].EnableWindow(FALSE);
}

///----------------------------------------------------------------------------
/// trySaveData - Confirm data is valid, and if it is save it. This function
/// should not be called directly and instead trySave should be called instead.
/// @return true if the data was valid, false if it was not.
///----------------------------------------------------------------------------

bool EditStoryDialog::trySaveData() {
    storyText = WtoA(txtStory.GetWindowText());
    summaryText = WtoA(txtSummary.GetWindowText());
    return true;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// moveControls - Move the controls into their proper positions
///----------------------------------------------------------------------------

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