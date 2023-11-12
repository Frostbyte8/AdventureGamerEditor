#include "editcharacter_tabviews.h"
#include "../../model/gamecharacter.h"

#include "../shared_functions.h"
#include "../../adventuregamer_constants.h"

namespace ControlIDs {
    const WORD Energy       = 101;
    const WORD Skill        = 102;
    const WORD Willpower    = 103;
    const WORD Luck         = 104;
    const WORD SightType    = 105;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterAttributesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if(ctrlID >= ControlIDs::Energy &&
           ctrlID <= ControlIDs::Luck) {

            const WORD index = ctrlID - ControlIDs::Energy;
            
            if(notifyCode == EN_KILLFOCUS) {

                // If the user inputs a bogus value, we will reset the field to the
                // minimum attribute value.

                int newValue = AdventureGamerConstants::MinAttributeValue;

                try {
                    newValue = std::stoi(WtoA(txtAttribType[index].GetWindowText()).c_str());
                }
                catch(const std::invalid_argument&) {}
                catch(const std::out_of_range&) {}
               
                spnAttribType[index].SetPos(newValue);
                parentWindow->madeChange();

            }
            else if(notifyCode == EN_CHANGE) {
                parentWindow->madeChange();
            }

        }
        else if(ctrlID == ControlIDs::SightType) {
            if(notifyCode == CBN_SELCHANGE) {
                parentWindow->madeChange();
            }
        }
    }
    else {
        return FALSE;
    }

    return TRUE;
}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the tab, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterAttributesTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);
    
    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpAttrib.Create(*this, 0, BS_GROUPBOX);

    SetWindowTextFromLangMapString("CharacterAttributesGroup", grpAttrib, caption, langMap);

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        lblAttribType[i].Create(*this, 0, SS_SIMPLE);
        txtAttribType[i].Create(*this, 0, ES_AUTOHSCROLL);
        txtAttribType[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtAttribType[i].LimitText(2);
        txtAttribType[i].SetDlgCtrlID(ControlIDs::Energy + i);
        spnAttribType[i].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY | UDS_NOTHOUSANDS |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);
        
        spnAttribType[i].SetRange(AdventureGamerConstants::MinAttributeValue,
                                  AdventureGamerConstants::MaxAttributeValue);

        attributeValidator[i] = IntegerValidator(&txtAttribType[i], AdventureGamerConstants::MinAttributeValue,
                                                 AdventureGamerConstants::MaxAttributeValue);

    }

    SetWindowTextFromLangMapString("EnergyCharacter", lblAttribType[0], caption, langMap);
    SetWindowTextFromLangMapString("SkillCharacter", lblAttribType[1], caption, langMap);
    SetWindowTextFromLangMapString("WillpowerCharacter", lblAttribType[2], caption, langMap);
    SetWindowTextFromLangMapString("LuckCharacter", lblAttribType[3], caption, langMap);

    lblSight.Create(*this, 0, SS_SIMPLE);
    SetWindowTextFromLangMapString("CharacterSightLabel", lblSight, caption, langMap);

    cbxSight.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL | WS_TABSTOP);
    cbxSight.SetDlgCtrlID(ControlIDs::SightType);

    AddStringFromLangMap("SightCharBlindOption", cbxSight, caption, langMap);
    AddStringFromLangMap("SightCharNormalOption", cbxSight, caption, langMap);
    AddStringFromLangMap("SightCharInfraredOption", cbxSight, caption, langMap);

    return retVal;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterAttributesTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::calculatePageWidth(const WindowMetrics& windowMetrics) {
    
    pageWidth = 0;

    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             lblAttribType[i].GetWindowTextW().c_str()), pageWidth);
    } 

    pageWidth += CS.XGROUPBOX_MARGIN * 2;

}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::insertData(GameCharacter::Builder& builder) {

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        
        int amount = 0;

#ifdef _DEBUG
        // The data was previously validated, so unless a programming error occured
        // this should not fail.
        try {
#endif // _DEBUG
            amount = std::stoi(WtoA(txtAttribType[i].GetWindowText()).c_str());
#ifdef _DEBUG
        }
        catch(const std::invalid_argument&) {
            assert(0);
        }
        catch(const std::out_of_range&) {
            assert(0);
        }
#endif // _DEBUG 

        builder.attribute(amount, i);

    }

    builder.sight(cbxSight.GetCurSel() + 1);
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::moveControls(const WindowMetrics& windowMetrics) {
    
    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();
    
    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultTextSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
        CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {

        lblAttribType[i].MoveWindow(cPos.x, cPos.y, 
                                    defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtAttribType[i].MoveWindow(cPos.x, cPos.y,
                                    defaultTextSize.cx, defaultTextSize.cy);

        cPos.Offset(0, defaultTextSize.cy + CS.YLABELASSOC_MARGIN);

        spnAttribType[i].SetBuddy(txtAttribType[i].GetHwnd());
    }

    lblSight.MoveWindow(cPos.x, cPos.y,
                        defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

    cbxSight.MoveWindow(cPos.x, cPos.y,
                        defaultTextSize.cx, CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, CD.YDROPDOWN);

    grpAttrib.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                         maxGroupBoxWidth, cPos.y);


    pageHeight = grpAttrib.GetClientRect().Height() + CS.YUNRELATED_MARGIN;

}

///----------------------------------------------------------------------------
/// populateFields - 
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::populateFields(const GameCharacter& gameCharacter) {

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        spnAttribType[i].SetPos(gameCharacter.getAttribute(i));
    }

    cbxSight.SetCurSel(gameCharacter.getSight() - 1);

}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterAttributesTab::validateFields() {

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        if(!attributeValidator[i].validate()) {
            return &attributeValidator[i];
        }
    }

    return NULL;
}
