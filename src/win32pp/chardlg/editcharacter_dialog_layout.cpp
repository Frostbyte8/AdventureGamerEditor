#include "editcharacter_dialog.h"
#include "editcharacter_tabviews.h"
#include "../shared_functions.h"

using namespace EditCharacterDialogConstants;

//=============================================================================
//
// Descriptions Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the tab, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterDescriptionsTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("CharDescriptionsGroup", grpDescriptions, caption, langMap);

    for (int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        lblDescriptions[i].Create(*this, 0, SS_SIMPLE);
        txtDescriptions[i].Create(*this, 0, ES_AUTOHSCROLL | WS_TABSTOP);
        txtDescriptions[i].SetExStyle(WS_EX_CLIENTEDGE);

        // For the last two in the range, we will give them browse buttons.
        if (i > 3) {
            btnBrowse[i - 4].Create(*this, 0, BS_PUSHBUTTON);
            btnBrowse[i - 4].SetDlgCtrlID(DescriptionsTab::ControlIDs::BrowseIcon + (i - 4));

            txtDescriptions[i].EnableWindow(FALSE);
        } else {
            txtDescriptions[i].SetDlgCtrlID(DescriptionsTab::ControlIDs::NameText + i);
        }
    }

    SetWindowTextFromLangMapString("CharNameLabel", lblDescriptions[0], caption, langMap);
    SetWindowTextFromLangMapString("CharOnSightLabel", lblDescriptions[1], caption, langMap);
    SetWindowTextFromLangMapString("CharOnFightLabel", lblDescriptions[2], caption, langMap);
    SetWindowTextFromLangMapString("CharOnDeathLabel", lblDescriptions[3], caption, langMap);
    SetWindowTextFromLangMapString("CharIconLabel", lblDescriptions[4], caption, langMap);
    SetWindowTextFromLangMapString("CharSoundLabel", lblDescriptions[5], caption, langMap);
    SetWindowTextFromLangMapString("BrowseButton", btnBrowse[0], caption, langMap);
    SetWindowTextFromLangMapString("BrowseButton", btnBrowse[1], caption, langMap);

    std::vector<std::string> imageExtensions;
    imageExtensions.push_back(".ICO");
    imageExtensions.push_back(".BMP");

    iconDescValidator = StringValidator(&txtDescriptions[4], NULL, &imageExtensions, 12, 5);
    soundDescValidator = StringValidator(&txtDescriptions[5], "", ".WAV", 12, 5);

    return retVal;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterDescriptionsTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// calculateWidth - Calculates the width of the tab page.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for (int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
            lblDescriptions[i].GetWindowTextW().c_str()), pageWidth);
    }

    // And we need to makes sure that we indicate that there is a margin for the group box
    // and a margin for the window edge.

    const WindowMetrics::ControlSpacing cs = windowMetrics.GetControlSpacing();
    pageWidth += (cs.XGROUPBOX_MARGIN * 2) + (cs.XWINDOW_MARGIN * 2);

}

///----------------------------------------------------------------------------
/// moveControls - Move/resize the controls to their desired positions
/// @param a constant reference to the dialog's window metrics object to be used
/// to move and size the controls.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::moveControls(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();
    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    // Since we have many labels and edit boxes on this tab page that take up 
    // the full width, we'll calculate their size here.

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultEditSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    // Start positioning them at the top of the groupbox past it's margins.

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    // Now loop through the 6 labels/edit boxes and move them

    for (int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {

        lblDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

        // For the last 2, we also need to make sure their buttons are moved too.

        if (i > GameCharacterDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameCharacterDescriptions::NumDescriptions;

            btnBrowse[currentButton].MoveWindow(cPos.x, cPos.y,
                                                CD.XBUTTON, CD.YBUTTON);

            if (i != 5) {
                cPos.Offset(0, CD.YBUTTON + CS.YRELATED_MARGIN);
            } else {
                cPos.Offset(0, CD.YBUTTON);
            }

        }

    }

    // Finally resize the groupbox, and update how tall the content on the tab page
    // is.

    grpDescriptions.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                               maxGroupBoxWidth, cPos.y);


    pageHeight = cPos.y + CS.YUNRELATED_MARGIN + CS.YWINDOW_MARGIN;
}

//=============================================================================
//
// Qualities Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the tab, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterQualitiesTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpFlags.Create(*this, 0, BS_GROUPBOX);

    SetWindowTextFromLangMapString("ChararacterFlagsGroup", grpFlags,
                                   caption, langMap);

    for (int k = 0; k < GameCharacterFlags::NumFlags; ++k) {

        btnFlags[k].Create(*this, 0, BS_AUTOCHECKBOX | WS_TABSTOP);

        btnFlags[k].SetDlgCtrlID(QualitiesTab::ControlIDs::FlagEnterDark + k);

    }

    SetWindowTextFromLangMapString("CanEnterDarkSpaces", btnFlags[0], caption, langMap);
    SetWindowTextFromLangMapString("CanEnterHazardSpaces", btnFlags[1], caption, langMap);
    SetWindowTextFromLangMapString("CanUseJumppads", btnFlags[2], caption, langMap);
    SetWindowTextFromLangMapString("CanBypassGates", btnFlags[3], caption, langMap);
    SetWindowTextFromLangMapString("CanBypassDoors", btnFlags[4], caption, langMap);
    SetWindowTextFromLangMapString("CanClimbThings", btnFlags[5], caption, langMap);
    SetWindowTextFromLangMapString("CharacterWanders", btnFlags[6], caption, langMap);
    SetWindowTextFromLangMapString("CharacterStalksPlayer", btnFlags[7], caption, langMap);

    grpProperties.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("CharPropertiesGroup", grpProperties, caption, langMap);

    lblMoney.Create(*this, 0, SS_SIMPLE);
    SetWindowTextFromLangMapString("MoneyCharHas", lblMoney, caption, langMap);

    txtMoney.Create(*this, 0, ES_AUTOHSCROLL);
    txtMoney.SetExStyle(WS_EX_CLIENTEDGE);
    txtMoney.LimitText(5);
    txtMoney.SetDlgCtrlID(QualitiesTab::ControlIDs::MoneyOnHand);
    spnMoney.Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY | UDS_NOTHOUSANDS |
                    UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

    lblType.Create(*this, 0, SS_SIMPLE);
    SetWindowTextFromLangMapString("TypeOfCharacter", lblType, caption, langMap);

    cbxType.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL |
                   WS_VSCROLL | WS_TABSTOP);

    cbxType.SetDlgCtrlID(QualitiesTab::ControlIDs::CharType);

    AddStringFromLangMap("TypeFighter", cbxType, caption, langMap);
    AddStringFromLangMap("TypeTrader", cbxType, caption, langMap);
    AddStringFromLangMap("TypeMissionary", cbxType, caption, langMap);

    spnMoney.SetRange(AdventureGamerConstants::MinObjectMonetaryValue,
                      AdventureGamerConstants::MaxObjectMonetaryValue);

    moneyValidator = IntegerValidator(&txtMoney,
                                      AdventureGamerConstants::MinObjectMonetaryValue,
                                      AdventureGamerConstants::MaxObjectMonetaryValue);

    return retVal;

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterQualitiesTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    pageWidth = 0;

    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();

    const LONG checkboxWidth = CD.XCHECKBOX + CD.XCHECKBOX_GAP;

    for (int i = 0; i < GameCharacterFlags::NumFlags; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
            btnFlags[i].GetWindowTextW().c_str()) + checkboxWidth,
            pageWidth);
    }

    pageWidth = std::max(windowMetrics.CalculateStringWidth(
        lblMoney.GetWindowTextW().c_str()), pageWidth);

    pageWidth = std::max(windowMetrics.CalculateStringWidth(
        lblType.GetWindowTextW().c_str()), pageWidth);

}


///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::moveControls(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultCheckboxSize(maxRowWidth, CD.YCHECKBOX);
    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultTextSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    // First deal with all the Flags

    for (int i = 0; i < GameCharacterFlags::NumFlags; ++i) {
        btnFlags[i].MoveWindow(cPos.x, cPos.y,
                               defaultCheckboxSize.cx, defaultCheckboxSize.cy);

        if (i == GameCharacterFlags::NumFlags - 1) {
            cPos.Offset(0, defaultCheckboxSize.cy);
        } else {
            cPos.Offset(0, defaultCheckboxSize.cy + CS.YRELATED_MARGIN);
        }
    }

    grpFlags.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                        maxGroupBoxWidth, cPos.y);


    // Now deal with money, and the character type

    cPos.Offset(0, CS.YUNRELATED_MARGIN + CS.YFIRST_GROUPBOX_MARGIN);

    lblMoney.MoveWindow(cPos.x, cPos.y,
                        defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

    txtMoney.MoveWindow(cPos.x, cPos.y,
                        defaultTextSize.cx, defaultTextSize.cy);

    spnMoney.SetBuddy(txtMoney);
    cPos.Offset(0, defaultTextSize.cy + CS.YRELATED_MARGIN);

    lblType.MoveWindow(cPos.x, cPos.y,
                       defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

    cbxType.MoveWindow(cPos.x, cPos.y, defaultTextSize.cx,
                       CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, CD.YDROPDOWN);

    grpProperties.MoveWindow(CS.XWINDOW_MARGIN,
                             grpFlags.GetClientRect().Height() + CS.YUNRELATED_MARGIN,
                             maxGroupBoxWidth, cPos.y - grpFlags.GetClientRect().Height());

    pageHeight = cPos.y + CS.YUNRELATED_MARGIN;

}

//=============================================================================
//
// Attributes Tab
//
//=============================================================================

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

    for (int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        lblAttribType[i].Create(*this, 0, SS_SIMPLE);
        txtAttribType[i].Create(*this, 0, ES_AUTOHSCROLL);
        txtAttribType[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtAttribType[i].LimitText(2);
        txtAttribType[i].SetDlgCtrlID(AttributesTab::ControlIDs::Energy + i);
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

    cbxSight.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP);

    cbxSight.SetDlgCtrlID(AttributesTab::ControlIDs::SightType);

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

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    pageWidth = 0;

    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();

    for (int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
            lblAttribType[i].GetWindowTextW().c_str()), pageWidth);
    }

    pageWidth += CS.XGROUPBOX_MARGIN * 2;

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

    for (int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {

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

//=============================================================================
//
// Misc Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the tab, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterMiscTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpLocations.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("CharLocationGroup", grpLocations,
                                   caption, langMap);

    for (int i = 0; i < 2; ++i) {
        lblCoords[i].Create(*this, 0, SS_SIMPLE);
        txtCoords[i].Create(*this, 0, ES_AUTOHSCROLL);
        txtCoords[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtCoords[i].SetDlgCtrlID(MiscTab::ControlIDs::XTextCoord + i);
    }

    SetWindowTextFromLangMapString("XCoord", lblCoords[0], caption, langMap);
    SetWindowTextFromLangMapString("YCoord", lblCoords[1], caption, langMap);

    grpInventory.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("CharInventoryLabel", grpInventory,
                                   caption, langMap);

    lsbInventory.Create(*this, 0, LBS_STANDARD | WS_VSCROLL |
                        LBS_NOINTEGRALHEIGHT | LBS_DISABLENOSCROLL);

    coordValidator[0] = IntegerValidator(&txtCoords[0], 0,
                                         gameMap->getWidth() - 1);

    coordValidator[1] = IntegerValidator(&txtCoords[1], 0,
                                         gameMap->getHeight() - 1);

    return retVal;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterMiscTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterMiscTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditCharacterMiscTab::calculatePageWidth(const WindowMetrics& windowMetrics) {
    pageWidth = 0;
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditCharacterMiscTab::moveControls(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    // M is the widest character, so we'll use that to measure how wide the label
    // needs to be.
    // Numbers tend to be fixed-width in size, so 4 digits is how much we'll
    // calculate for

    const int coordWidth = windowMetrics.CalculateStringWidth(L"MM");
    const int txtWidth = windowMetrics.CalculateStringWidth(L"0000");

    for (int i = 0; i < 2; ++i) {
        lblCoords[i].MoveWindow(cPos.x, cPos.y, coordWidth, CD.YTEXTBOX_ONE_LINE_ALONE);
        cPos.Offset(coordWidth, 0);
        txtCoords[i].MoveWindow(cPos.x, cPos.y, txtWidth, CD.YTEXTBOX_ONE_LINE_ALONE);
        cPos.Offset(txtWidth + CS.XRELATED_MARGIN, 0);
    }

    cPos.x = CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN;
    cPos.y += CD.YTEXTBOX_ONE_LINE_ALONE;

    grpLocations.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                            maxGroupBoxWidth, cPos.y);

    cPos.Offset(0, CS.YUNRELATED_MARGIN + CS.YFIRST_GROUPBOX_MARGIN);

    lsbInventory.MoveWindow(cPos.x, cPos.y,
                            maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE * 5);

    cPos.Offset(0, -CS.YFIRST_GROUPBOX_MARGIN);

    grpInventory.MoveWindow(CS.XWINDOW_MARGIN, cPos.y,
                            maxGroupBoxWidth, CS.YFIRST_GROUPBOX_MARGIN + (CD.YTEXTBOX_ONE_LINE_ALONE * 5) + CS.YLAST_GROUPBOX_MARGIN);

    pageHeight = grpInventory.GetClientRect().Height() + CS.YUNRELATED_MARGIN;

}