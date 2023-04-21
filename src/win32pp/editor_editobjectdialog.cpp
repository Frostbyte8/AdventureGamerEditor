#include "editor_editobjectdialog.h"

void EditObjectDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

int EditObjectDialog::OnCreate(CREATESTRUCT& cs) {
    tabControl.Create(*this);
    tabControl.AddTabPage(new EditObjectDescriptionsTab(windowMetrics), L"Descriptions", 101);
    tabControl.MoveWindow(0, 0, 128, 128, TRUE);
    return CWnd::OnCreate(cs);
}

LRESULT EditObjectDialog::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    if(msg == WM_SIZE) {
        return OnSize(wParam, lParam);
    }

    return WndProcDefault(msg, wParam, lParam);
}

///----------------------------------------------------------------------------
/// OnSize - Processes the WM_SIZE message for the dialog window
///----------------------------------------------------------------------------

LRESULT EditObjectDialog::OnSize(WPARAM& wParam, LPARAM& lParam) {

    const WORD newWidth = LOWORD(lParam);
    const WORD newHeight = HIWORD(lParam);

    bool hasHorz = false;
    bool hasVert = false;
    int whichBars = 0;

    if(newWidth < contentSize.cx) {
        hasHorz = true;
    }

    if(newHeight < contentSize.cy) {
        hasVert = true;
    }

    // Check to see if we need a horizontal one after turning
    // on the vertical one
    if(hasHorz == false && hasVert == true) {
        if(newWidth < contentSize.cx - 17) {
            hasHorz = true;
        }

    }
    else if(hasVert == false && hasHorz == true) {
        if(newHeight < contentSize.cy - 17) {
            hasVert = true;
        }
    }

    if(hasHorz && hasVert) {
        ShowScrollBar(SB_BOTH, TRUE);
    }
    else if(hasHorz) {
        ShowScrollBar(SB_HORZ, TRUE);
        ShowScrollBar(SB_VERT, FALSE);
    }
    else if(hasVert) {
        ShowScrollBar(SB_VERT, TRUE);
        ShowScrollBar(SB_HORZ, FALSE);
    }
    else {
        ShowScrollBar(SB_BOTH, FALSE);
    }

    return 0;

}

//
//
//

int EditObjectDescriptionsTab::OnCreate(CREATESTRUCT& cs) {
    const int retVal = CWnd::OnCreate(cs);

    grpDescriptions.Create(*this, 0, WS_CLIPSIBLINGS | BS_GROUPBOX);
    grpDescriptions.SetWindowText(L"Descriptions");

    lblDescriptions.Create(*this, 0, SS_SIMPLE);
    lblDescriptions.SetWindowText(L"Name");
    txtDescriptions.Create(*this, WS_EX_CLIENTEDGE, ES_AUTOHSCROLL);

    return retVal;
}
