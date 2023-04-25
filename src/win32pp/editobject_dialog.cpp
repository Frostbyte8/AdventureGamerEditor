#include "editobject_dialog.h"

void EditObjectDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

int EditObjectDialog::OnCreate(CREATESTRUCT& cs) {

    const WindowMetrics::ControlSpacing ctrlSpace = windowMetrics->GetControlSpacing();

    tabControl.Create(*this);
    descriptionsTab = reinterpret_cast<EditObjectDescriptionsTab*>(tabControl.AddTabPage(new EditObjectDescriptionsTab(windowMetrics), L"Descriptions", 101));
    tabControl.MoveWindow(ctrlSpace.XWINDOW_MARGIN, ctrlSpace.YWINDOW_MARGIN, 450, 480, TRUE);

    // TODO: Figure out tab width
    descriptionsTab->moveControls();
    HFONT dialogFont = windowMetrics->GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetFontTest), (LPARAM)dialogFont);
    return CWnd::OnCreate(cs);
}

LRESULT EditObjectDialog::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    if(msg == WM_SIZE) {
        return OnSize(wParam, lParam);
    }

    return WndProcDefault(msg, wParam, lParam);
}

bool CALLBACK EditObjectDialog::SetFontTest(HWND child, LPARAM font) {
    ::SendMessage(child, WM_SETFONT, font, true);
    return true;
}

///----------------------------------------------------------------------------
/// OnSize - Processes the WM_SIZE message for the dialog window
///----------------------------------------------------------------------------

LRESULT EditObjectDialog::OnSize(WPARAM& wParam, LPARAM& lParam) {

    /*
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
    */

    return 0;

}

//
//
//