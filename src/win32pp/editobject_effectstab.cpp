#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include <algorithm>

#include "shared_functions.h"
#include "../adventuregamer_constants.h"

namespace ControlIDs {
    const WORD EnergyBase            = 101;
    const WORD EnergyRandom          = 102;
    const WORD SkillBase             = 103;
    const WORD SkillRandom           = 104;
    const WORD WillpowerBase         = 105;
    const WORD WillpowerRandom       = 106;
    const WORD LuckBase              = 107;
    const WORD LuckRandom            = 108;
    const WORD TorchLifeBase         = 109;
    const WORD TorchLifeRandom       = 110;
    const WORD MakesSight            = 111;
    const WORD MakesHearing          = 112;
    const WORD EffectsConsume        = 113;
    const WORD EffectsTemp           = 114;
    const WORD EnergyPosBTN          = 115;
    const WORD EnergyNegBTN          = 116;
    const WORD SkillPosBTN           = 117;
    const WORD SkillNegBTN           = 118;
    const WORD WillpowerPosBTN       = 119;
    const WORD WillpowerNegBTN       = 120;
    const WORD LuckPosBTN            = 121;
    const WORD LuckNegBTN            = 122;
    const WORD TorchPosBTN           = 123;
    const WORD TorchNegBTN           = 124;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditObjectEffectsTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if(ctrlID >= ControlIDs::EnergyBase &&
           ctrlID <= ControlIDs::TorchLifeRandom) {

            if(notifyCode == EN_KILLFOCUS) {
                updateAttributeValue(ctrlID);
                return TRUE;
            }
            else if(notifyCode == EN_CHANGE) {
                parentWindow->madeChange();
            }

        }
        else if(ctrlID >= ControlIDs::EffectsConsume && ctrlID <= ControlIDs::EffectsTemp) {
            if(notifyCode == BN_CLICKED) {
                parentWindow->madeChange();
            }
        }
        else if(ctrlID >= ControlIDs::MakesSight && ctrlID <= ControlIDs::MakesHearing) {
            if(notifyCode == CBN_SELCHANGE) {
                parentWindow->madeChange();
            }
        }
        else if(ctrlID >= ControlIDs::EnergyPosBTN && ctrlID <= ControlIDs::TorchNegBTN) {
            if(notifyCode == BN_CLICKED) {
                parentWindow->madeChange();
            }
        }
    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectEffectsTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpEffects.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText("EffectsOnPlayerGroupLabel", grpEffects,
                      caption, langMap);

    btnEffect[0].Create(*this, 0, BS_AUTORADIOBUTTON | WS_GROUP);
    btnEffect[1].Create(*this, 0, BS_AUTORADIOBUTTON);
    btnEffect[0].SetDlgCtrlID(ControlIDs::EffectsConsume);
    btnEffect[1].SetDlgCtrlID(ControlIDs::EffectsTemp);

    EOD_SetWindowText("EffectsConsumpativeLabel", btnEffect[0], caption, langMap);
    EOD_SetWindowText("EffectsReversedLabel", btnEffect[1], caption, langMap);

    grpAttrib.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText("AttributesGroupLabel", grpAttrib, caption, langMap);

    for(int k = 0; k < 5; ++k) {

        lblAttribType[k].Create(*this, 0, SS_CENTER);

        btnAttribPolarity[k*2].Create(*this, 0, WS_GROUP | BS_AUTORADIOBUTTON);
        btnAttribPolarity[(k*2)+1].Create(*this, 0, BS_AUTORADIOBUTTON);

        btnAttribPolarity[k*2].SetDlgCtrlID((ControlIDs::EnergyPosBTN) + (k*2));
        btnAttribPolarity[(k*2)+1].SetDlgCtrlID((ControlIDs::EnergyPosBTN) + (k*2) + 1);

        EOD_SetWindowText("PositiveSignLabel", btnAttribPolarity[k*2], caption, langMap);

        EOD_SetWindowText("NegativeSignLabel", btnAttribPolarity[(k*2)+1], caption, langMap);

        for(int l = 0; l < 2; ++l) {
            txtAttribAmount[(k*2)+l].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
            spnAttribAmount[(k*2)+l].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                    UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

            txtAttribAmount[(k*2)+l].SetExStyle(WS_EX_CLIENTEDGE);
            txtAttribAmount[(k*2)+l].LimitText(2);

            spnAttribAmount[(k*2)+l].SetRange(AdventureGamerConstants::MinAttributeValue,
                                        AdventureGamerConstants::MaxAttributeValue);

            txtAttribAmount[(k*2)+l].SetDlgCtrlID(ControlIDs::EnergyBase+((k*2)+l));
        }
    }

    EOD_SetWindowText("EnergyLabel", lblAttribType[0], caption, langMap);
    EOD_SetWindowText("SkillLabel", lblAttribType[1], caption, langMap);
    EOD_SetWindowText("WillpowerLabel", lblAttribType[2], caption, langMap);
    EOD_SetWindowText("LuckLabel", lblAttribType[3], caption, langMap);
    EOD_SetWindowText("TorchLifeLabel", lblAttribType[4], caption, langMap);

    for(int i = 0; i < 2; ++i) {
        lblAttribHeading[i].Create(*this, 0, SS_CENTER);
    }
    EOD_SetWindowText("BaseAttribLabel", lblAttribHeading[0], caption, langMap);
    EOD_SetWindowText("RandomAttribLabel", lblAttribHeading[1], caption, langMap);

    for(int m = 0; m < 2; ++m) {
        lblSenses[m].Create(*this, 0, SS_SIMPLE);
        cbxSenses[m].Create(*this, 0, CBS_DROPDOWNLIST | WS_TABSTOP);
        cbxSenses[m].SetDlgCtrlID(ControlIDs::MakesSight+m);
    }

    EOD_AddString("SightNoEffectOption", cbxSenses[0], caption, langMap);
    EOD_AddString("SightBlindOption", cbxSenses[0], caption, langMap);
    EOD_AddString("SightNormalOption", cbxSenses[0], caption, langMap);
    EOD_AddString("SightInfraredOption", cbxSenses[0], caption, langMap);

    EOD_AddString("HearingNoEffectOption", cbxSenses[1], caption, langMap);
    EOD_AddString("HearingDeafOption", cbxSenses[1], caption, langMap);
    EOD_AddString("HearingNormalOption", cbxSenses[1], caption, langMap);
    EOD_AddString("HearingUltrasonicOption", cbxSenses[1], caption, langMap);

    EOD_SetWindowText("MakesPlayerSightLabel", lblSenses[0], caption, langMap);
    
    EOD_SetWindowText("MakesPlayerHaeringLabel", lblSenses[1], caption, langMap);

    return retVal;

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

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectEffectsTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();
    const int RadioButtonBaseWidth = CD.XRADIOBUTTON + CD.XRADIOBUTTON_GAP;

    // See if attributes is the widest
    const int headWidth1 = windowMetrics.CalculateStringWidth(lblAttribHeading[0].GetWindowText().c_str());
    const int headWidth2 = windowMetrics.CalculateStringWidth(lblAttribHeading[1].GetWindowText().c_str());

    int typeWidth = 0;

    for(int i = 0; i < 5; ++i) {
        const int textSize = windowMetrics.CalculateStringWidth(lblAttribType[i].GetWindowTextW().c_str());
        typeWidth = std::max(textSize, typeWidth);
    }

    const int polarityWidth = windowMetrics.CalculateStringWidth(btnAttribPolarity[0].GetWindowText().c_str()) +
                              windowMetrics.CalculateStringWidth(btnAttribPolarity[1].GetWindowText().c_str()) +
                              RadioButtonBaseWidth;
    
    if(polarityWidth > typeWidth) {
        typeWidth = polarityWidth;
    }
    
    // The Attrib width must be the longest string/buttons times 3 so everything is even
    const int totalAttribWidth = (std::max(std::max(headWidth1, headWidth2), typeWidth)) * 3;

    // Now just to figure out if any other string is wider than the above.

    int singleLineWidth = 0;

    for(int k = 0; k < 2; ++k) {

        int stringWidth = windowMetrics.CalculateStringWidth(btnEffect[k].GetWindowText().c_str());
        singleLineWidth = std::max(stringWidth + RadioButtonBaseWidth, singleLineWidth);

        stringWidth = windowMetrics.CalculateStringWidth(lblSenses[k].GetWindowText().c_str());
        singleLineWidth = std::max(stringWidth, singleLineWidth);
    }

    pageWidth = std::max(singleLineWidth, totalAttribWidth);
    
    pageWidth += CS.XGROUPBOX_MARGIN * 2;   
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditObjectEffectsTab::insertData(GameObject::Builder& builder) {

    if(btnEffect[1].GetCheck() == BST_CHECKED) {
        builder.flags2(builder.getFlags2() | GameObjectFlags2::EffectsTemporary);
    }

    int baseAmount = 0;
    int randAmount = 0;

    for(int i = 0; i < 5; ++i) {

#ifdef _DEBUG
        // The data was previously validated, so unless a programming error occured
        // this should not fail.
        try {
#endif // _DEBUG

            baseAmount = std::stoi(WtoA(txtAttribAmount[i*2].GetWindowText()).c_str());
            randAmount = std::stoi(WtoA(txtAttribAmount[(i*2)+1].GetWindowText()).c_str());

#ifdef _DEBUG
        }
        catch(const std::invalid_argument&) {
            assert(0);
        }
        catch(const std::out_of_range&) {
            assert(0);
        }
#endif // _DEBUG 

        if(btnAttribPolarity[i*2].GetCheck() == BST_CHECKED) {
            baseAmount *= -1;
            randAmount *= -1;
        }

        builder.attributeBase(baseAmount, i);
        builder.attributeRandom(randAmount, i);

    } 

    builder.makesSight(cbxSenses[0].GetCurSel());
    builder.makesHearing(cbxSenses[1].GetCurSel());

}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectEffectsTab::moveControls(const WindowMetrics& windowMetrics) {
    
    const WindowMetrics::ControlSpacing CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();

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

    const int firstColumnWidth  = (columnWidth * 3 == maxRowWidth) 
                                  ? columnWidth 
                                  : maxRowWidth - (columnWidth * 2); 

    cPos.Offset(0, CS.YUNRELATED_MARGIN + CS.YFIRST_GROUPBOX_MARGIN);

    for(int k = 0; k < 5; ++k) {

        lblAttribType[k].MoveWindow(cPos.x, cPos.y,
                                    firstColumnWidth, CD.YLABEL);

        if(k == 0) {

            CPoint headerPos(cPos);
            headerPos.Offset(firstColumnWidth, 0);
            lblAttribHeading[0].MoveWindow(headerPos.x, headerPos.y,
                                           columnWidth, CD.YLABEL);
            
            headerPos.Offset(columnWidth, 0 );
            lblAttribHeading[1].MoveWindow(headerPos.x, headerPos.y,
                                           columnWidth, CD.YLABEL);
        }

        cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);
        CPoint polarPos(cPos);

        for(int l = 0; l < 2; ++l) {

            btnAttribPolarity[(k*2)+l].MoveWindow(polarPos.x, polarPos.y,
                                                  firstColumnWidth / 2, CD.YRADIOBUTTON);

            polarPos.Offset(firstColumnWidth - (firstColumnWidth / 2), 0);

        }

        polarPos = cPos;
        polarPos.Offset(firstColumnWidth, 0);

        for(int m = 0; m < 2; ++m) {

            txtAttribAmount[(k*2)+m].MoveWindow(polarPos.x, polarPos.y,
                                                columnWidth, CD.YTEXTBOX_ON_BUTTON_ROW);

            spnAttribAmount[(k*2)+m].SetBuddy(txtAttribAmount[(k*2)+m].GetHwnd());
            polarPos.Offset(columnWidth, 0);

        }

        cPos.Offset(0, CD.YTEXTBOX_ON_BUTTON_ROW);
    }

    for(int n = 0; n < 2; n++) {

        cPos.Offset(0, CS.YRELATED_MARGIN);
        lblSenses[n].MoveWindow(cPos.x, cPos.y,
                                defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YRELATED_MARGIN);
        cbxSenses[n].MoveWindow(cPos.x, cPos.y, maxRowWidth,
                                CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

        cPos.Offset(0, CD.YDROPDOWN);

    }

    cPos.Offset(0, CS.YLAST_GROUPBOX_MARGIN);
    const int yPos = grpEffects.GetClientRect().Height() + CS.YUNRELATED_MARGIN;

    grpAttrib.MoveWindow(CS.XWINDOW_MARGIN, yPos,
                         maxGroupBoxWidth, cPos.y - yPos);

    pageHeight = cPos.y + CS.YUNRELATED_MARGIN;

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectEffectsTab::populateFields(const GameObject& gameObject, const GameMap& gameMap) {

    if(gameObject.getFlags2() & GameObjectFlags2::EffectsTemporary) {
        btnEffect[1].SetCheck(BST_CHECKED);
    }
    else {
        btnEffect[0].SetCheck(BST_CHECKED);
    }

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

    cbxSenses[0].SetCurSel(gameObject.getMakesSight());
    cbxSenses[1].SetCurSel(gameObject.getMakesHearing());
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// updateAttributeValue - Update the spinner control to match the value in
/// the textbox. Note that the spinner control will auto-cap the value if it
/// exceeds the valid range.
/// @param ID of the textbox control that is associated with the spinner
/// control to be updated
///----------------------------------------------------------------------------

void EditObjectEffectsTab::updateAttributeValue(const WORD& ctrlID) {

    const int ctrlIndex = ctrlID - ControlIDs::EnergyBase; 

    // If the user some how inputs an invalid number, we will reset
    // the value back to the minimum attribute value.

    int newValue = AdventureGamerConstants::MinAttributeValue;

    try {
        newValue = std::stoi(WtoA(txtAttribAmount[ctrlIndex].GetWindowText()).c_str());
    }
    catch(const std::invalid_argument&) {}
    catch(const std::out_of_range&) {}

    spnAttribAmount[ctrlIndex].SetPos(newValue);

}
