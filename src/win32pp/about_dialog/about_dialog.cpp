#include "about_dialog.h"
#include "../../util/languagemapper.h"
#include "../shared_functions.h"
#include "../../gitinfo.h"
#include "../../credits_and_thanks.h"
#include "../../resources/resource.h"

namespace ControlIDs {

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
        if(ctrlID == IDOK) {
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

    lblProgramName.Create(*this, 0, SS_CENTER);
    lblProgramVersion.Create(*this, 0, SS_CENTER);
    lblProgramCopyright.Create(*this, 0, SS_CENTER);
    lblProgramIcon.Create(*this, 0, SS_ICON);

    txtCredits.Create(*this, 0, ES_MULTILINE | ES_READONLY | WS_VSCROLL);
    txtCredits.SetExStyle(WS_EX_CLIENTEDGE);

    btnOkay.Create(*this, 0, 0);

    programIcon = GetApp()->LoadIcon(IDI_APPICON);
    lblProgramIcon.SetIcon(programIcon);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    caption = LM_toUTF8("ProgramName", langMap);
    lblProgramName.SetWindowText(caption);

    caption = AtoW(langMap.get("VersionLabel").c_str(), CP_UTF8);
    caption += " ";
    caption += AtoW(GIT_VERSION_INFO.c_str(), CP_UTF8);
    lblProgramVersion.SetWindowText(caption);

    caption = AtoW(CreditsAndThanks::copyright.c_str(), CP_UTF8);
    lblProgramCopyright.SetWindowText(caption);

    caption = AtoW(CreditsAndThanks::programCredits.c_str(), CP_UTF8);
    txtCredits.SetWindowText(caption);
    
    caption = LM_toUTF8("OKButton", langMap);
    btnOkay.SetWindowTextW(caption);
    btnOkay.SetDlgCtrlID(IDOK);

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

    // TODO: Find the actual minimum width, specifically, we need to make sure the longest link is
    // visible in the edit box.

    const int minWidth = (CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3) + (CS.XWINDOW_MARGIN * 2);
    const int boundryWidth = minWidth - CS.XWINDOW_MARGIN * 2;

    CPoint cPos(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN);

    lblProgramName.MoveWindow(cPos.x, cPos.y, boundryWidth, CD.YLABEL);
    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);

    // TODO: How do we handle a icon when it needs to be scaled? :)
    lblProgramIcon.MoveWindow((minWidth / 2) - 16, cPos.y, 32, 32);
    cPos.Offset(0, lblProgramIcon.GetClientRect().Height() + CS.YRELATED_MARGIN);

    lblProgramVersion.MoveWindow(cPos.x, cPos.y, boundryWidth, CD.YLABEL);
    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);

    lblProgramCopyright.MoveWindow(cPos.x, cPos.y, boundryWidth, CD.YLABEL);
    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);;

    txtCredits.MoveWindow(cPos.x, cPos.y, boundryWidth, CD.YTEXTBOX_ONE_LINE_ALONE + (CD.YTEXTBOX_ADDITIONAL_LINES * 5));
    cPos.Offset(boundryWidth - (CD.XBUTTON), txtCredits.GetClientRect().Height() + CS.YUNRELATED_MARGIN);

    btnOkay.MoveWindow((minWidth / 2) - (CD.XBUTTON / 2), cPos.y, CD.XBUTTON, CD.YBUTTON);


    cPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    
    // TODO: Finish calculating dimensions
    RECT rc = {0, 0, minWidth, cPos.y + CS.YWINDOW_MARGIN + CD.YBUTTON};

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);
    

}
