#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include <algorithm>

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectDescriptionsTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    grpDescriptions.SetWindowTextW(L"Descriptions");

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; i++) {
        
        lblDescriptions[i].Create(*this, 0, SS_SIMPLE);
        txtDescriptions[i].Create(*this, WS_EX_CLIENTEDGE, WS_TABSTOP | ES_AUTOHSCROLL);
        txtDescriptions[i].SetExStyle(WS_EX_CLIENTEDGE);
        
        EOD_SetWindowText(LanguageConstants::NameLabel+i, lblDescriptions[i], caption, langMap);

        // For the last two fields, we need browse buttons
        if(i > GameObjectDescriptions::NumDescriptions - 1) {
            btnBrowse[i - GameObjectDescriptions::NumDescriptions].Create(*this, 0, WS_TABSTOP | BS_PUSHBUTTON);
            EOD_SetWindowText(LanguageConstants::BrowseButton, btnBrowse[i - GameObjectDescriptions::NumDescriptions], caption, langMap);
        }

    }

    txtDescriptions[GameObjectDescriptions::Icon].EnableWindow(FALSE);
    txtDescriptions[GameObjectDescriptions::Sound].EnableWindow(FALSE);

    calculatePageWidth();

    return retVal;

}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::calculatePageWidth() {
    
    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {
        pageWidth = std::max(windowMetrics->CalculateStringWidth(lblDescriptions[i].GetWindowTextW().c_str()), pageWidth);
    }

    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();
    pageWidth += (cs.XGROUPBOX_MARGIN * 2) + (cs.XWINDOW_MARGIN * 2);
    
}

///----------------------------------------------------------------------------
/// calculatePageHeight - Finds how tall the tab page needs to be to display
/// the controls
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::calculatePageHeight() {
    // The bottom of the groupbox is exactly how tall the page is.
    pageHeight = grpDescriptions.GetClientRect().bottom;
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::moveControls() {
    
    const WindowMetrics::ControlSpacing cs      = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions cd   = windowMetrics->GetControlDimensions();
    
    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (cs.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (cs.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, cd.YLABEL);
    const CSize defaultEditSize(maxRowWidth, cd.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(cs.XGROUPBOX_MARGIN + cs.XWINDOW_MARGIN, 
                cs.YFIRST_GROUPBOX_MARGIN + cs.YRELATED_MARGIN);

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {

        lblDescriptions[i].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy, FALSE);
        cPos.Offset(0, defaultLabelSize.cy + cs.YLABELASSOC_MARGIN);
        txtDescriptions[i].MoveWindow(cPos.x, cPos.y, defaultEditSize.cx, defaultEditSize.cy, FALSE);
        cPos.Offset(0, defaultEditSize.cy + cs.YRELATED_MARGIN);

        if(i > GameObjectDescriptions::NumDescriptions -1) {
            btnBrowse[i - GameObjectDescriptions::NumDescriptions].MoveWindow(cPos.x, cPos.y, cd.XBUTTON, cd.YBUTTON);

            if(i != 5) {
                cPos.Offset(0, cd.YBUTTON + cs.YRELATED_MARGIN);
            }
            else {
                cPos.Offset(0, cd.YBUTTON + cs.YLAST_GROUPBOX_MARGIN);
            }

        }

    }

    grpDescriptions.MoveWindow(cs.XWINDOW_MARGIN, cs.YRELATED_MARGIN,
                               maxGroupBoxWidth, cPos.y);

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::populateFields(const GameObject &gameObject) {

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {
        CString caption;
        EOD_SetWindowText(gameObject.getDescription(i), txtDescriptions[i], caption);
    }

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectDescriptionsTabPage";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectDescriptionsTab::PreTranslateMessage(MSG &msg) {

    if(IsDialogMessage(msg)) {
        return TRUE;
    }
    
    return CWnd::PreTranslateMessage(msg);

}