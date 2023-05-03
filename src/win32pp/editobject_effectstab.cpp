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
    EOD_SetWindowText(LanguageConstants::EffectsOnAttribGroup, grpEffects, caption, langMap);

    btnEffect[0].Create(*this, 0, BS_AUTORADIOBUTTON | WS_GROUP);
    btnEffect[1].Create(*this, 0, BS_AUTORADIOBUTTON);

    for(int i = 0; i < 2; ++i) {
        EOD_SetWindowText(LanguageConstants::ConsumpativeLabel+i, btnEffect[i], caption, langMap);
    }

    grpAttrib.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText(LanguageConstants::ObjectAttributesGroup, grpAttrib, caption, langMap);

    for(int k = 0; k < 5; ++k) {

        lblAttribType[k].Create(*this, 0, SS_CENTER);
        EOD_SetWindowText(LanguageConstants::ObjectEnergyLabel+k, lblAttribType[k], caption, langMap);
        btnAttribPolarity[(k*2)].Create(*this, 0, WS_GROUP | BS_AUTORADIOBUTTON);
        btnAttribPolarity[(k*2)+1].Create(*this, 0, BS_AUTORADIOBUTTON);

        EOD_SetWindowText(LanguageConstants::PositiveSignLabel, btnAttribPolarity[k*2], caption, langMap);
        EOD_SetWindowText(LanguageConstants::NegativeSignLabel, btnAttribPolarity[(k*2)+1], caption, langMap);

    }

    for(int i = 0; i < 2; ++i) {
        lblAttribHeading[i].Create(*this, 0, SS_CENTER);
        EOD_SetWindowText(LanguageConstants::ObjectBaseAmountLabel+i, lblAttribHeading[i], caption, langMap);
    }

    for(int l = 0; l < 10; ++l) {
        txtAttribAmount[l].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER);
        spnAttribAmount[l].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

        txtAttribAmount[l].SetExStyle(WS_EX_CLIENTEDGE);

        spnAttribAmount[l].SetRange(GameObjectConstants::MinAttributeValue,
                                    GameObjectConstants::MaxAttributeValue);
    }

    for(int m = 0; m < 2; ++m) {
        lblSenses[m].Create(*this, 0, SS_SIMPLE);
        cbxSenses[m].Create(*this, 0, CBS_DROPDOWN);
    }

    for(int n = 0; n < 4; ++n) {
        EOD_AddString(LanguageConstants::SightNoEffectOption+n, cbxSenses[0], caption, langMap);
        EOD_AddString(LanguageConstants::HearingNoEffectOption+n, cbxSenses[1], caption, langMap);
    }

    EOD_SetWindowText(LanguageConstants::MakesPlayersSightLabel, lblSenses[0], caption, langMap);
    EOD_SetWindowText(LanguageConstants::MakesPlayersHearingLabel, lblSenses[1], caption, langMap);

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

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
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
        cbxSenses[n].MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));
        cPos.Offset(0, CD.YDROPDOWN);
    }

    cPos.Offset(0, CS.YLAST_GROUPBOX_MARGIN);
    const int yPos = grpEffects.GetClientRect().Height() + CS.YUNRELATED_MARGIN;

    grpAttrib.MoveWindow(CS.XWINDOW_MARGIN, yPos, maxGroupBoxWidth, cPos.y - yPos);

    pageHeight = cPos.y + CS.YUNRELATED_MARGIN;
}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectEffectsTab::populateFields(const GameObject& gameObject) {

    for(int i = 0; i < 5; ++i) {

        int baseAmount      = gameObject.getAttributeBase(i);
        int randomAmount    = gameObject.getAttributeRandom(i);
        int polarAmount     = (baseAmount < 0 ? 1 : 0);

        btnAttribPolarity[(i*2)+polarAmount].SetCheck(BST_CHECKED);

        CString caption = AtoW(std::to_string(abs(baseAmount)).c_str());
        txtAttribAmount[(i*2)].SetWindowText(caption);
        caption = AtoW(std::to_string(abs(randomAmount)).c_str());
        txtAttribAmount[(i*2)+1].SetWindowText(caption);
    }
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
