#include "editobject_tabviews.h"

#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include "../util/languagemapper.h"
#include <algorithm>

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectQualitiesTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);
    //LanguageMapper& langMap = LanguageMapper::getInstance();
    //CString caption;
    calculatePageWidth();

    return retVal;

}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::calculatePageWidth() {
    
    pageWidth = 0;
    
}

///----------------------------------------------------------------------------
/// calculatePageHeight - Finds how tall the tab page needs to be to display
/// the controls
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::calculatePageHeight() {
    pageHeight = 0;
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::moveControls() {
    
    const WindowMetrics::ControlSpacing cs      = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions cd   = windowMetrics->GetControlDimensions();
    
    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (cs.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (cs.XGROUPBOX_MARGIN * 2);

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::populateFields(const GameObject &gameObject) {
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectQualitiesTabPage";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectQualitiesTab::PreTranslateMessage(MSG &msg) {

    if(IsDialogMessage(msg)) {
        return TRUE;
    }
    
    return CWnd::PreTranslateMessage(msg);

}