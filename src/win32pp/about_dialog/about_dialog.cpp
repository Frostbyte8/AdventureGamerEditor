#include "about_dialog.h"
#include "../../util/languagemapper.h"
//#include "../shared_functions.h"

namespace ControlIDs {
    const WORD SummaryText  = 101;
    const WORD StoryText    = 102;
}

//=============================================================================
// Constructors
//=============================================================================

AboutDialog::AboutDialog(MainWindowInterface* inMainWindow, HWND inParentHandle) : 
EditDialogBase(inMainWindow, inParentHandle, &MainWindowInterface::finishedAboutDialog), dialogCaption("") {
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL AboutDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    const WORD ctrlID = LOWORD(wParam);
    const WORD notifyCode = HIWORD(wParam);

    if(lParam) {
        if(ctrlID == IDOK || ctrlID == IDCANCEL || ctrlID == DefControlIDs::IDAPPLY) {
            if(notifyCode == BN_CLICKED) {
                dialogButtonPressed(ctrlID);
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

int AboutDialog::OnCreate(CREATESTRUCT& cs) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;


    /*
    lblSummary.Create(*this, 0, SS_SIMPLE);
    
    SetWindowTextFromLangMapString("SummaryLabel", lblSummary, caption, langMap);

    txtSummary.Create(*this, 0, ES_MULTILINE | ES_WANTRETURN | WS_TABSTOP | WS_VSCROLL);
    txtSummary.SetExStyle(WS_EX_CLIENTEDGE);
    txtSummary.LimitText(GameMapConstants::MaxSummaryText);
    txtSummary.SetDlgCtrlID(ControlIDs::SummaryText);

    lblStory.Create(*this, 0, SS_SIMPLE);
    SetWindowTextFromLangMapString("StoryLabel", lblStory, caption, langMap);

    txtStory.Create(*this, 0, ES_MULTILINE | ES_WANTRETURN | WS_TABSTOP | WS_VSCROLL);
    txtStory.SetExStyle(WS_EX_CLIENTEDGE);
    txtStory.LimitText(GameMapConstants::MaxStoryText);
    txtStory.SetDlgCtrlID(ControlIDs::SummaryText);
    */

    createDefaultDialogButtons(true);

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    moveControls();
    
    return CWnd::OnCreate(cs);
}


///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for the dialog
///----------------------------------------------------------------------------

void AboutDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"About Dialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// moveControls - Move the controls into their proper positions
///----------------------------------------------------------------------------

void AboutDialog::moveControls() {

    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();
    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();

    const int minWidth = (CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3) + (CS.XWINDOW_MARGIN * 2);

    /*
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
        btnDialog[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        cPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }
    */
    
    // TODO: Finish calculating dimensions
    RECT rc = {0, 0, minWidth, 123 + CS.YWINDOW_MARGIN + CD.YBUTTON};

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);
    

}
