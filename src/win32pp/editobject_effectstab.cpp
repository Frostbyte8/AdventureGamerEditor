#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include <algorithm>

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectEffectsTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpEffects.Create(*this, 0, BS_GROUPBOX);
    grpEffects.SetWindowText(L"Effects on Player Attributes");

    for(int i = 0; i < 2; ++i) {
        btnEffect[i].Create(*this, 0, BS_RADIOBUTTON);
        EOD_SetWindowText(LanguageConstants::NameLabel+i, btnEffect[i], caption, langMap);
    }

    grpAttrib.Create(*this, 0, BS_GROUPBOX);
    grpAttrib.SetWindowText(L"Attributes");

    for(int k = 0; k < 5; ++k) {
        lblAttribType[k].Create(*this, 0, SS_SIMPLE);
        btnAttribPolarity[(k*2)].Create(*this, 0, BS_RADIOBUTTON | WS_GROUP);
        btnAttribPolarity[(k*2)].SetWindowText(L"+");
        btnAttribPolarity[(k*2)+1].Create(*this, 0, BS_RADIOBUTTON);
        btnAttribPolarity[(k*2)+1].SetWindowText(L"-");
    }

    lblAttribHeading[0].Create(*this, 0, SS_SIMPLE);
    lblAttribHeading[1].Create(*this, 0, SS_SIMPLE);

    for(int l = 0; l < 10; ++l) {
        txtAttribAmount[l].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER);
        spnAttribAmount[l].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

    }

    for(int m = 0; m < 2; ++m) {
        lblSenses[m].Create(*this, 0, SS_SIMPLE);
        cbxSenses[m].Create(*this, 0, CBS_DROPDOWN);
    }

    // This is just for debugging the width caclulation

    lblAttribHeading[0].SetWindowText(L"Minimum (0 - 12)");
    lblAttribHeading[1].SetWindowText(L"Maximum (0 - 12)");

    lblAttribType[0].SetWindowText(L"Energy");
    lblAttribType[1].SetWindowText(L"Skill");
    lblAttribType[2].SetWindowText(L"Willpower");
    lblAttribType[3].SetWindowText(L"Luck");
    lblAttribType[4].SetWindowText(L"Torch Life");

    /*
    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    grpDescriptions.SetWindowTextW(L"Descriptions");

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; i++) {
        
        lblDescriptions[i].Create(*this, 0, SS_SIMPLE);
        txtDescriptions[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
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
    */

    calculatePageWidth();

    return retVal;

}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectEffectsTab::calculatePageWidth() {

    const WindowMetrics::ControlDimensions CD = windowMetrics->GetControlDimensions();
    const int RadioButtonBaseWidth = CD.XRADIOBUTTON + CD.XRADIOBUTTON_GAP;

    // See if attributes is the widest
    const int headWidth1 = windowMetrics->CalculateStringWidth(lblAttribHeading[0].GetWindowText().c_str());
    const int headWidth2 = windowMetrics->CalculateStringWidth(lblAttribHeading[1].GetWindowText().c_str());

    int typeWidth = 0;

    for(int i = 0; i < 5; ++i) {
        const int textSize = windowMetrics->CalculateStringWidth(lblAttribType[i].GetWindowTextW().c_str());
        typeWidth = std::max(textSize, typeWidth);
    }

    const int polarityWidth = windowMetrics->CalculateStringWidth(btnAttribPolarity[0].GetWindowText().c_str()) +
                              windowMetrics->CalculateStringWidth(btnAttribPolarity[1].GetWindowText().c_str()) +
                              RadioButtonBaseWidth;
    
    if(polarityWidth > typeWidth) {
        typeWidth = polarityWidth;
    }
    
    // The Attrib width must be the longest string/buttons times 3 so everything is even
    const int totalAttribWidth = (std::max(std::max(headWidth1, headWidth2), typeWidth)) * 3;

    // Now just to figure out if any other string is wider than the above.

    int singleLineWidth = 0;

    for(int k = 0; k < 2; ++k) {

        int stringWidth = windowMetrics->CalculateStringWidth(btnEffect[k].GetWindowText().c_str());
        singleLineWidth = std::max(stringWidth + RadioButtonBaseWidth, singleLineWidth);

        stringWidth = windowMetrics->CalculateStringWidth(lblSenses[k].GetWindowText().c_str());
        singleLineWidth = std::max(stringWidth, singleLineWidth);
    }

    pageWidth = std::max(singleLineWidth, totalAttribWidth);
    pageWidth = pageWidth;
    
}

///----------------------------------------------------------------------------
/// calculatePageHeight - Finds how tall the tab page needs to be to display
/// the controls
///----------------------------------------------------------------------------

void EditObjectEffectsTab::calculatePageHeight() {
    pageHeight = 0;
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectEffectsTab::moveControls() {
    
    const WindowMetrics::ControlSpacing cs      = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions cd   = windowMetrics->GetControlDimensions();
    
    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (cs.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (cs.XGROUPBOX_MARGIN * 2);
    const int columnWidth       = maxGroupBoxWidth / 3;
    
    const int firstColumnWidth = (columnWidth * 3 == maxGroupBoxWidth) ? columnWidth : maxGroupBoxWidth - (columnWidth * 2); 
   

    const CSize defaultLabelSize(maxRowWidth, cd.YLABEL);
    const CSize defaultEditSize(maxRowWidth, cd.YTEXTBOX_ONE_LINE_ALONE);

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectEffectsTab::populateFields(const GameObject& gameObject) {

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditObjectEffectsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectEffectsTabPage";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectEffectsTab::PreTranslateMessage(MSG &msg) {

    if(IsDialogMessage(msg)) {
        return TRUE;
    }
    
    return CWnd::PreTranslateMessage(msg);

}
