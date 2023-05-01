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

    btnEffect[0].Create(*this, 0, BS_AUTORADIOBUTTON | WS_GROUP);
    btnEffect[1].Create(*this, 0, BS_AUTORADIOBUTTON);
    EOD_SetWindowText(LanguageConstants::NameLabel+1, btnEffect[0], caption, langMap);
    EOD_SetWindowText(LanguageConstants::NameLabel+2, btnEffect[1], caption, langMap);

    grpAttrib.Create(*this, 0, BS_GROUPBOX);
    grpAttrib.SetWindowText(L"Attributes");

    for(int k = 0; k < 5; ++k) {
        lblAttribType[k].Create(*this, 0, SS_CENTER);
        btnAttribPolarity[(k*2)].Create(*this, 0, WS_GROUP | BS_AUTORADIOBUTTON);
        btnAttribPolarity[(k*2)].SetWindowText(L"+");
        btnAttribPolarity[(k*2)+1].Create(*this, 0, BS_AUTORADIOBUTTON);
        btnAttribPolarity[(k*2)+1].SetWindowText(L"-");
    }

    lblAttribHeading[0].Create(*this, 0, SS_CENTER);
    lblAttribHeading[1].Create(*this, 0, SS_CENTER);

    for(int l = 0; l < 10; ++l) {
        txtAttribAmount[l].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER);
        spnAttribAmount[l].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

        txtAttribAmount[l].SetExStyle(WS_EX_CLIENTEDGE);

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
    lblSenses[0].SetWindowText(L"Hearing");
    lblSenses[1].SetWindowText(L"Sight");

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
    
    const WindowMetrics::ControlSpacing CS = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics->GetControlDimensions();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultRadioSize(maxRowWidth, CD.YRADIOBUTTON);

    CPoint cPos(CS.XBUTTON_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);
    
    // We'll move the first two radio buttons that effect how the object works first

    for(int i = 0; i < 2; ++i) {

        btnEffect[i].MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YRADIOBUTTON);

        cPos.Offset(0, CD.YRADIOBUTTON);

        if(i != 1) {
            cPos.Offset(0, CS.YRELATED_MARGIN);
        }

    } 

    grpEffects.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN, maxGroupBoxWidth, cPos.y);

    // Now for the grid of controls

    const int columnWidth       = maxRowWidth / 3;
    const int firstColumnWidth = (columnWidth * 3 == maxRowWidth) ? columnWidth : maxRowWidth - (columnWidth * 2); 

    cPos.Offset(0, CS.YUNRELATED_MARGIN + CS.YFIRST_GROUPBOX_MARGIN);

    for(int k = 0; k < 5; ++k) {

        lblAttribType[k].MoveWindow(cPos.x, cPos.y, firstColumnWidth, CD.YLABEL);

        if(k == 0) {

            CPoint headerPos(cPos);
            headerPos.Offset(firstColumnWidth, 0);
            lblAttribHeading[0].MoveWindow(headerPos.x, headerPos.y, columnWidth, CD.YLABEL);
            headerPos.Offset(columnWidth, 0 );
            lblAttribHeading[1].MoveWindow(headerPos.x, headerPos.y, columnWidth, CD.YLABEL);
        }

        cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);
        CPoint polarPos(cPos);

        for(int l = 0; l < 2; ++l) {
            btnAttribPolarity[(k*2)+l].MoveWindow(polarPos.x, polarPos.y, firstColumnWidth / 2, CD.YRADIOBUTTON);
            polarPos.Offset(firstColumnWidth - (firstColumnWidth / 2), 0);
        }

        polarPos = cPos;
        polarPos.Offset(firstColumnWidth, 0);

        for(int m = 0; m < 2; ++m) {
            txtAttribAmount[(k*2)+m].MoveWindow(polarPos.x, polarPos.y, columnWidth, CD.YTEXTBOX_ON_BUTTON_ROW);
            spnAttribAmount[(k*2)+m].SetBuddy(txtAttribAmount[(k*2)+m].GetHwnd());
            polarPos.Offset(columnWidth, 0);
        }

        cPos.Offset(0, CD.YTEXTBOX_ON_BUTTON_ROW);
    }

    for(int n = 0; n < 2; n++) {
        cPos.Offset(0, CS.YRELATED_MARGIN);
        lblSenses[n].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy);
        cPos.Offset(0, defaultLabelSize.cy + CS.YRELATED_MARGIN);
        cbxSenses[n].MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YCOMBOBOX);
        cPos.Offset(0, CD.YCOMBOBOX);
    }

    cPos.Offset(0, CS.YLAST_GROUPBOX_MARGIN);
    const int yPos = grpEffects.GetClientRect().Height() + CS.YUNRELATED_MARGIN;


    grpAttrib.MoveWindow(CS.XWINDOW_MARGIN, yPos, maxGroupBoxWidth, cPos.y - yPos);

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
