#include "editobject_tabviews.h"
#include "../util/languagemapper.h"
#include <algorithm>

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

#define EOD_SETWIDGETTEXT(ID, WIDGET) caption = AtoW(languageMapper.get(ID).c_str(), CP_UTF8); \
    WIDGET.SetWindowTextW(caption);

int EditObjectDescriptionsTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& languageMapper = LanguageMapper::getInstance();
    CString caption;

    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    grpDescriptions.SetWindowTextW(L"Descriptions");

    for(int i = 0; i < 6; i++) {
        lblDescriptions[i].Create(*this, 0, SS_SIMPLE | WS_BORDER);
        txtDescriptions[i].Create(*this, WS_EX_CLIENTEDGE, ES_AUTOHSCROLL | WS_BORDER);
        EOD_SETWIDGETTEXT(LanguageConstants::NameLabel+i, lblDescriptions[i]);
    }

    for(int k = 0; k < 2; k++) {
        btnBrowse[k].Create(*this, 0, BS_PUSHBUTTON);
        EOD_SETWIDGETTEXT(LanguageConstants::BrowseButton, btnBrowse[k]);
    }

    calculatePageWidth();

    return retVal;

}

#undef EODIALOG_SETWINDOWTEXT

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::calculatePageWidth() {
    
    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for(int i = 0; i < 6; ++i) {
        pageWidth = std::max(windowMetrics->CalculateStringWidth(lblDescriptions[i].GetWindowTextW().c_str()), pageWidth);
    }

    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();
    pageWidth += (cs.XGROUPBOX_MARGIN * 2);
    
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
    
    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions cd = windowMetrics->GetControlDimensions();
    
    const int maxRowWidth = (GetClientRect().right - (cs.XGROUPBOX_MARGIN * 2)) - (cs.XWINDOW_MARGIN * 2);

    CPoint cPos(cs.XGROUPBOX_MARGIN + cs.XRELATED_MARGIN, 
                cs.YFIRST_GROUPBOX_MARGIN + cs.YRELATED_MARGIN);

    const CSize defaultLabelSize(maxRowWidth, cd.YLABEL);
    const CSize defaultEditSize(maxRowWidth, cd.YTEXTBOX_ONE_LINE_ALONE);

    for(int i = 0; i < 4; ++i) {
        lblDescriptions[i].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy, FALSE);
        cPos.Offset(0, defaultLabelSize.cy + cs.YLABELASSOC_MARGIN);
        txtDescriptions[i].MoveWindow(cPos.x, cPos.y, defaultEditSize.cx, defaultEditSize.cy, FALSE);
        cPos.Offset(0, defaultEditSize.cy + cs.YRELATED_MARGIN);
    }

    for(int k = 4; k < 6; ++k) {
        lblDescriptions[k].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy, FALSE);
        cPos.Offset(0, defaultLabelSize.cy + cs.YLABELASSOC_MARGIN);
        txtDescriptions[k].MoveWindow(cPos.x, cPos.y, defaultEditSize.cx, defaultEditSize.cy, FALSE);
        cPos.Offset(0, defaultEditSize.cy + cs.YRELATED_MARGIN);
        btnBrowse[k-4].MoveWindow(cPos.x, cPos.y, cd.XBUTTON, cd.YBUTTON);
        cPos.Offset(0, cd.YBUTTON + cs.YRELATED_MARGIN);
    }

    cPos.Offset(0, (-(cs.YRELATED_MARGIN) + cs.YLAST_GROUPBOX_MARGIN));
    grpDescriptions.MoveWindow(cs.XRELATED_MARGIN, cs.YRELATED_MARGIN, maxRowWidth + (cs.XGROUPBOX_MARGIN * 2), cPos.y);



}