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
    //EOD_SETWIDGETTEXT(LanguageConstants::DescriptionGroupCaption, grpDescriptions);

    for(int i = 0; i < 6; i++) {
        lblDescriptions[i].Create(*this, 0, SS_SIMPLE | WS_BORDER);
        txtDescriptions[i].Create(*this, WS_EX_CLIENTEDGE, ES_AUTOHSCROLL);
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
/// calculatePageWidth - Finds the max dimensions that the tab will take up
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::calculatePageWidth() {
    
    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels.

    for(int i = 0; i < 6; i++) {
        pageWidth = std::max(windowMetrics->CalculateStringWidth(lblDescriptions[i].GetWindowTextW().c_str()), pageWidth);
    }

    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();
    pageWidth += (cs.XGROUPBOX_MARGIN * 2);
    
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::moveControls() {
    
    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions cd = windowMetrics->GetControlDimensions();
    
    const int maxRowWidth = GetClientRect().right - (cs.XGROUPBOX_MARGIN * 2);

    CPoint cPos(cs.XGROUPBOX_MARGIN, cs.YFIRST_GROUPBOX_MARGIN);
    CSize cSize(maxRowWidth, cd.YTEXTBOX_ONE_LINE_ALONE);

    lblDescriptions[0].MoveWindow(cPos.x, cPos.y, cSize.cx, cSize.cy, FALSE);    

}