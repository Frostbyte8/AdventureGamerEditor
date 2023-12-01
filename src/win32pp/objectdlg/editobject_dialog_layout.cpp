#include "editobject_dialog.h"
#include "../shared_functions.h"
#include "../../model/gameobject.h"

#include <algorithm>

using namespace EditObjectDialogConstants;

//=============================================================================
//
// Dialog Window
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditObjectDialog::OnCreate(CREATESTRUCT& createStruct) {

    const int NUM_DIALOG_BUTTONS = isEditObject ? 3 : 2;
    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    tabControl.Create(*this, 0, WS_TABSTOP);

    // Create the tab pages for the dialog

    EditDialogBase* parentWindow = reinterpret_cast<EditDialogBase*>(this);

    caption = LM_toUTF8("DescriptionTabLabel", langMap);
    descriptionsTab = reinterpret_cast<EditObjectDescriptionsTab*>(
        tabControl.AddTabPage(new EditObjectDescriptionsTab(parentWindow), caption));

    caption = LM_toUTF8("QualitiesTabLabel", langMap);
    qualitiesTab = reinterpret_cast<EditObjectQualitiesTab*>(
        tabControl.AddTabPage(new EditObjectQualitiesTab(gameMap, parentWindow, this), caption));

    caption = LM_toUTF8("EffectsTabLabel", langMap);
    effectsTab = reinterpret_cast<EditObjectEffectsTab*>(
        tabControl.AddTabPage(new EditObjectEffectsTab(parentWindow), caption));

    caption = LM_toUTF8("LocationsTabLabel", langMap);
    locationsTab = reinterpret_cast<EditObjectLocationsTab*>(
        tabControl.AddTabPage(new EditObjectLocationsTab(gameMap, parentWindow, this), caption));

    // We also need to create the Ok, Cancel and Apply buttons too.

    createDefaultDialogButtons(isEditObject);

    // Correct the tab order for the tab control
    tabControl.SetWindowPos(btnDialog[NUM_DIALOG_BUTTONS - 1], 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // The font must be set on the controls before we do any kind of sizing
    // so we can reliably test widths on the controls

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    // Move the controls into place, and display the first tab.

    moveControls();
    tabControl.SelectPage(0);
    tabControl.SetFocus();

    return CWnd::OnCreate(createStruct);

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditObjectDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls into their proper positions
///----------------------------------------------------------------------------

void EditObjectDialog::moveControls() {

    const WindowMetrics::ControlSpacing CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();
    const int numDialogButtons = isEditObject ? 3 : 2;

    // Find the widest point. We'll see what is longer:
    // The minimum dialog width, or the widest tab. 4 Buttons + Spacing seems like a good
    // amount of space.

    LONG widestPoint = findLongestTab(true);
    const LONG minSize = (CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3) + (CS.XWINDOW_MARGIN * 2);
    widestPoint = std::max(widestPoint, minSize);

    // Now that we know what our widest section is, we can resize our tab control
    // and resize the contents of the tab pages to fit.

    RECT tabRect ={ 0, 0, widestPoint, 0 };

    tabControl.SendMessage(TCM_ADJUSTRECT, TRUE, (LPARAM)&tabRect);

    const LONG adjustedPageWidth = tabRect.right + abs(tabRect.left);

    tabControl.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                          adjustedPageWidth, 0, FALSE);

    // The tab pages won't have the correct width, so we need to change this
    // or everything will appear to be invisible.
    // TODO: This might break if the DPI changes.

    descriptionsTab->MoveWindow(0, 0, adjustedPageWidth, 0, FALSE);
    qualitiesTab->MoveWindow(0, 0, adjustedPageWidth, 0, FALSE);
    effectsTab->MoveWindow(0, 0, adjustedPageWidth, 0, FALSE);
    locationsTab->MoveWindow(0, 0, adjustedPageWidth, 0, FALSE);

    // Move controls so they can fit into their new width.
    descriptionsTab->moveControls(windowMetrics);
    qualitiesTab->moveControls(windowMetrics);
    effectsTab->moveControls(windowMetrics);
    locationsTab->moveControls(windowMetrics);

    // Now we can figure out how tall the tab control needs to be, and adjust
    // the dimensions of the tab control accordingly.

    const LONG tallestPage = findLongestTab(false);
    tabControl.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                          adjustedPageWidth, tallestPage, FALSE);

    // Finally, we need to move the dialog buttons into place
    CPoint cPos(CS.XWINDOW_MARGIN + adjustedPageWidth,
                CS.YWINDOW_MARGIN + tallestPage + CS.YUNRELATED_MARGIN);

    cPos.Offset(-(CD.XBUTTON), 0);

    // We have to go backwards though to place them
    for (int i = numDialogButtons - 1; i >= 0; --i) {
        btnDialog[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        cPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }

    // Finally, Resize the dialog window, but be careful not to move it or
    // activate it in anyway. TODO: DPI may require a call to center this,
    // so we may need monitor information of some kind.

    RECT rc ={ 0, 0,
        adjustedPageWidth + (CS.XWINDOW_MARGIN * 2),
        cPos.y + CD.YBUTTON + CS.YWINDOW_MARGIN };

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);
}

///----------------------------------------------------------------------------
/// findLongestTab - Finds the Widest/Tallest tab, and returns how long it is.
/// @param if true, find the width, if false, find the height
/// @returns the length of the tab in the given direction
///----------------------------------------------------------------------------

LONG EditObjectDialog::findLongestTab(const bool getWidth) {

    LONG longestTab = 0;
    std::vector<LONG> pageDims;

    const WindowMetrics::ControlSpacing CS = windowMetrics.GetControlSpacing();

    if (getWidth) {

        // TODO: Window metrics isn't needed?
        descriptionsTab->calculatePageWidth(windowMetrics);
        qualitiesTab->calculatePageWidth(windowMetrics);
        effectsTab->calculatePageWidth(windowMetrics);
        locationsTab->calculatePageWidth(windowMetrics);

        pageDims.push_back(descriptionsTab->getPageWidth());
        pageDims.push_back(qualitiesTab->getPageWidth());
        pageDims.push_back(effectsTab->getPageWidth());
        pageDims.push_back(locationsTab->getPageWidth());
    } else {

        // For height, it is calculated after all the controls are moved
        // into place.

        pageDims.push_back(descriptionsTab->getPageHeight());
        pageDims.push_back(qualitiesTab->getPageHeight());
        pageDims.push_back(effectsTab->getPageHeight());
        pageDims.push_back(locationsTab->getPageHeight());
    }

    const size_t numTabs = pageDims.size();

    for (size_t i = 0; i < numTabs; ++i) {
        longestTab = std::max(pageDims[i], longestTab);
    }

    // Apply margins.

    if (getWidth) {

        longestTab += CS.XWINDOW_MARGIN * 2;

    } else {
        longestTab += CS.YWINDOW_MARGIN * 2;
    }

    return longestTab;
}

//=============================================================================
//
// Descriptions Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectDescriptionsTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("DescriptionsGroupLabel", grpDescriptions, caption, langMap);

    for (int i = 0; i < GameObjectDescriptions::NumAllDescriptions; i++) {

        lblDescriptions[i].Create(*this, 0, SS_SIMPLE);
        txtDescriptions[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
        txtDescriptions[i].SetExStyle(WS_EX_CLIENTEDGE);

        if (i == 0) {
            txtDescriptions[i].LimitText(GameObjectConstants::MaxNameLength);
        } else {
            txtDescriptions[i].LimitText(GameObjectConstants::MaxDescriptionLength);
        }

        // For the last two fields, we need browse buttons
        if (i > GameObjectDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameObjectDescriptions::NumDescriptions;

            btnBrowse[currentButton].Create(*this, 0,
                                            WS_TABSTOP | BS_PUSHBUTTON);
            SetWindowTextFromLangMapString("BrowseButton", btnBrowse[currentButton], caption, langMap);
        } else {
            txtDescriptions[i].SetDlgCtrlID(DescriptionsTab::ControlIDs::Name + i);
        }

    }

    SetWindowTextFromLangMapString("ObjectNameLabel", lblDescriptions[0], caption, langMap);
    SetWindowTextFromLangMapString("OnObjectSightLabel", lblDescriptions[1], caption, langMap);
    SetWindowTextFromLangMapString("OnUseLabel", lblDescriptions[2], caption, langMap);
    SetWindowTextFromLangMapString("OnLastUseLabel", lblDescriptions[3], caption, langMap);
    SetWindowTextFromLangMapString("ObjectIconLabel", lblDescriptions[4], caption, langMap);
    SetWindowTextFromLangMapString("ObjectSoundLabel", lblDescriptions[5], caption, langMap);

    txtDescriptions[GameObjectDescriptions::Icon].EnableWindow(FALSE);
    txtDescriptions[GameObjectDescriptions::Sound].EnableWindow(FALSE);

    btnBrowse[0].SetDlgCtrlID(DescriptionsTab::ControlIDs::BrowseIcon);
    btnBrowse[1].SetDlgCtrlID(DescriptionsTab::ControlIDs::BrowseSound);

    std::vector<std::string> imageExtensions;
    imageExtensions.push_back(".ICO");
    imageExtensions.push_back(".BMP");

    iconDescValidator = StringValidator(&txtDescriptions[4], NULL, &imageExtensions, 12, 5);
    soundDescValidator = StringValidator(&txtDescriptions[5], "", ".WAV", 12, 5);

    return retVal;

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
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();

    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for (int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
            lblDescriptions[i].GetWindowTextW().c_str()), pageWidth);
    }

    const WindowMetrics::ControlSpacing cs = windowMetrics.GetControlSpacing();
    pageWidth += cs.XGROUPBOX_MARGIN * 2;

}

///----------------------------------------------------------------------------
/// moveControls - Move/resize the controls to their desired positions
/// @param a constant reference to the dialog's window metrics object to be used
/// to move and size the controls.
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::moveControls(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultEditSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    for (int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {

        lblDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

        if (i > GameObjectDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameObjectDescriptions::NumDescriptions;

            btnBrowse[currentButton].MoveWindow(cPos.x, cPos.y,
                                                CD.XBUTTON, CD.YBUTTON);

            if (i != 5) {
                cPos.Offset(0, CD.YBUTTON + CS.YRELATED_MARGIN);
            } else {
                cPos.Offset(0, CD.YBUTTON);
            }

        }

    }

    grpDescriptions.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                               maxGroupBoxWidth, cPos.y);

    pageHeight = cPos.y + CS.YWINDOW_MARGIN + CS.YUNRELATED_MARGIN;

}

//=============================================================================
//
// Qualities Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectQualitiesTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpFlags.Create(*this, 0, BS_GROUPBOX);

    SetWindowTextFromLangMapString("FlagsGroupLabel", grpFlags,
                                   caption, langMap);

    for (int k = 0; k < GameObjectFlags1::NumFlags; ++k) {

        btnFlags[k].Create(*this, 0, BS_AUTOCHECKBOX | WS_TABSTOP);
        btnFlags[k].SetDlgCtrlID(QualitiesTab::ControlIDs::MasterKey + k);

    }

    SetWindowTextFromLangMapString("IsMasterKeyLabel", btnFlags[0], caption, langMap);
    SetWindowTextFromLangMapString("IsInvisibleLabel", btnFlags[1], caption, langMap);
    SetWindowTextFromLangMapString("IsLadderLabel", btnFlags[2], caption, langMap);
    SetWindowTextFromLangMapString("ProtectsPlayerLabel", btnFlags[3], caption, langMap);
    SetWindowTextFromLangMapString("IsFlashlightLabel", btnFlags[4], caption, langMap);
    SetWindowTextFromLangMapString("IsWornLabel", btnFlags[5], caption, langMap);
    SetWindowTextFromLangMapString("IsFixedPosLabel", btnFlags[6], caption, langMap);
    SetWindowTextFromLangMapString("IsMoneyLabel", btnFlags[7], caption, langMap);

    for (int i = 0; i < 2; ++i) {

        lblProperties[i].Create(*this, 0, SS_SIMPLE);
        txtProperties[i].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        txtProperties[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtProperties[i].LimitText(5);

        // By overriding how the control is created, we need to specify the default
        // flags ourself, including WS_VISIBLE
        spnProperties[i].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY | UDS_NOTHOUSANDS |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

        txtProperties[i].SetDlgCtrlID(QualitiesTab::ControlIDs::UsesBox + i);

    }

    SetWindowTextFromLangMapString("ObjectWorthLabel", lblProperties[0], caption, langMap);
    SetWindowTextFromLangMapString("ObjectNumUsesLabel", lblProperties[1], caption, langMap);

    grpProperties.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("PropertiesGroupLabel", grpProperties, caption, langMap);

    lblProperties[2].Create(*this, 0, SS_SIMPLE);
    SetWindowTextFromLangMapString("ObjectHeldLabel", lblProperties[2], caption, langMap);

    cbxUsedWith.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL | WS_TABSTOP);
    AddStringFromLangMap("NoObjectSelectedOption", cbxUsedWith, caption, langMap);

    const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
    const size_t numObjects = gameObjects.size();

    for (size_t j = 0; j < numObjects; ++j) {
        caption = AtoW((std::to_string(gameObjects[j].getID()) + ". ").c_str());
        caption += AtoW(gameObjects[j].getName().c_str());
        cbxUsedWith.AddString(caption);
        cbxUsedWith.SetDlgCtrlID(QualitiesTab::ControlIDs::UsedWith);
    }

    spnProperties[0].SetRange(AdventureGamerConstants::MaxObjectMonetaryValue,
                              AdventureGamerConstants::MinObjectMonetaryValue);


    spnProperties[1].SetRange(AdventureGamerConstants::MaxNumUses,
                              AdventureGamerConstants::MinNumUses);

    moneyValidator = IntegerValidator(&txtProperties[0], AdventureGamerConstants::MinObjectMonetaryValue, AdventureGamerConstants::MaxObjectMonetaryValue);
    usesValidator = IntegerValidator(&txtProperties[1], AdventureGamerConstants::MinNumUses, AdventureGamerConstants::MaxNumUses);

    return retVal;

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
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    pageWidth = 0;

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    const LONG checkboxWidth = CD.XCHECKBOX + CD.XCHECKBOX_GAP;

    for (int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
            btnFlags[i].GetWindowTextW().c_str()) + checkboxWidth,
            pageWidth);
    }

    std::vector<CWnd *> controlList;
    controlList.reserve(2);

    for (int k = 0; k < 2; ++k) {
        controlList.push_back(&lblProperties[k]);
    }

    const size_t clSize = controlList.size();

    for (size_t j = 0; j < clSize; ++j) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
            controlList[j]->GetWindowTextW().c_str()), pageWidth);
    }

    pageWidth += CS.XGROUPBOX_MARGIN * 2;
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::moveControls(const WindowMetrics& windowMetrics) {

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

    for (int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        btnFlags[i].MoveWindow(cPos.x, cPos.y,
                               defaultCheckboxSize.cx, defaultCheckboxSize.cy);

        if (i == GameObjectFlags1::NumFlags - 1) {
            cPos.Offset(0, defaultCheckboxSize.cy);
        } else {
            cPos.Offset(0, defaultCheckboxSize.cy + CS.YRELATED_MARGIN);
        }
    }

    grpFlags.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                        maxGroupBoxWidth, cPos.y);

    // Now deal with things like money and uses

    cPos.Offset(0, CS.YUNRELATED_MARGIN + CS.YFIRST_GROUPBOX_MARGIN);

    for (int i = 0; i < 2; i++) {
        lblProperties[i].MoveWindow(cPos.x, cPos.y,
                                    defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtProperties[i].MoveWindow(cPos.x, cPos.y,
                                    defaultTextSize.cx, defaultTextSize.cy);

        spnProperties[i].SetBuddy(txtProperties[i]);
        cPos.Offset(0, defaultTextSize.cy + CS.YRELATED_MARGIN);
    }

    lblProperties[2].MoveWindow(cPos.x, cPos.y,
                                defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

    cbxUsedWith.MoveWindow(cPos.x, cPos.y, defaultTextSize.cx,
                           CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, CD.YDROPDOWN);

    grpProperties.MoveWindow(CS.XWINDOW_MARGIN,
                             grpFlags.GetClientRect().Height() + CS.YUNRELATED_MARGIN,
                             maxGroupBoxWidth, cPos.y - grpFlags.GetClientRect().Height());


    pageHeight = cPos.y + CS.YUNRELATED_MARGIN;

}

//=============================================================================
//
// Effects Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectEffectsTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpEffects.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("EffectsOnPlayerGroupLabel", grpEffects,
                                   caption, langMap);

    btnEffect[0].Create(*this, 0, BS_AUTORADIOBUTTON | WS_GROUP);
    btnEffect[1].Create(*this, 0, BS_AUTORADIOBUTTON);
    btnEffect[0].SetDlgCtrlID(EffectsTab::ControlIDs::EffectsConsume);
    btnEffect[1].SetDlgCtrlID(EffectsTab::ControlIDs::EffectsTemp);

    SetWindowTextFromLangMapString("EffectsConsumpativeLabel", btnEffect[0], caption, langMap);
    SetWindowTextFromLangMapString("EffectsReversedLabel", btnEffect[1], caption, langMap);

    grpAttrib.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("AttributesGroupLabel", grpAttrib, caption, langMap);

    for (int k = 0; k < 5; ++k) {

        lblAttribType[k].Create(*this, 0, SS_CENTER);

        btnAttribPolarity[k * 2].Create(*this, 0, WS_GROUP | BS_AUTORADIOBUTTON);
        btnAttribPolarity[(k * 2) + 1].Create(*this, 0, BS_AUTORADIOBUTTON);

        btnAttribPolarity[k * 2].SetDlgCtrlID((EffectsTab::ControlIDs::EnergyPosBTN) + (k * 2));
        btnAttribPolarity[(k * 2) + 1].SetDlgCtrlID((EffectsTab::ControlIDs::EnergyPosBTN) + (k * 2) + 1);

        SetWindowTextFromLangMapString("PositiveSignLabel", btnAttribPolarity[k * 2], caption, langMap);

        SetWindowTextFromLangMapString("NegativeSignLabel", btnAttribPolarity[(k * 2) + 1], caption, langMap);

        for (int l = 0; l < 2; ++l) {
            txtAttribAmount[(k * 2) + l].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
            spnAttribAmount[(k * 2) + l].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

            txtAttribAmount[(k * 2) + l].SetExStyle(WS_EX_CLIENTEDGE);
            txtAttribAmount[(k * 2) + l].LimitText(2);

            spnAttribAmount[(k * 2) + l].SetRange(AdventureGamerConstants::MinAttributeValue,
                                                  AdventureGamerConstants::MaxAttributeValue);

            txtAttribAmount[(k * 2) + l].SetDlgCtrlID(EffectsTab::ControlIDs::EnergyBase + ((k * 2) + l));
        }
    }

    SetWindowTextFromLangMapString("EnergyLabel", lblAttribType[0], caption, langMap);
    SetWindowTextFromLangMapString("SkillLabel", lblAttribType[1], caption, langMap);
    SetWindowTextFromLangMapString("WillpowerLabel", lblAttribType[2], caption, langMap);
    SetWindowTextFromLangMapString("LuckLabel", lblAttribType[3], caption, langMap);
    SetWindowTextFromLangMapString("TorchLifeLabel", lblAttribType[4], caption, langMap);

    for (int i = 0; i < 2; ++i) {
        lblAttribHeading[i].Create(*this, 0, SS_CENTER);
    }
    SetWindowTextFromLangMapString("BaseAttribLabel", lblAttribHeading[0], caption, langMap);
    SetWindowTextFromLangMapString("RandomAttribLabel", lblAttribHeading[1], caption, langMap);

    for (int m = 0; m < 2; ++m) {
        lblSenses[m].Create(*this, 0, SS_SIMPLE);
        cbxSenses[m].Create(*this, 0, CBS_DROPDOWNLIST | WS_TABSTOP);
        cbxSenses[m].SetDlgCtrlID(EffectsTab::ControlIDs::MakesSight + m);
    }

    AddStringFromLangMap("SightNoEffectOption", cbxSenses[0], caption, langMap);
    AddStringFromLangMap("SightBlindOption", cbxSenses[0], caption, langMap);
    AddStringFromLangMap("SightNormalOption", cbxSenses[0], caption, langMap);
    AddStringFromLangMap("SightInfraredOption", cbxSenses[0], caption, langMap);

    AddStringFromLangMap("HearingNoEffectOption", cbxSenses[1], caption, langMap);
    AddStringFromLangMap("HearingDeafOption", cbxSenses[1], caption, langMap);
    AddStringFromLangMap("HearingNormalOption", cbxSenses[1], caption, langMap);
    AddStringFromLangMap("HearingUltrasonicOption", cbxSenses[1], caption, langMap);

    SetWindowTextFromLangMapString("MakesPlayerSightLabel", lblSenses[0], caption, langMap);

    SetWindowTextFromLangMapString("MakesPlayerHearingLabel", lblSenses[1], caption, langMap);

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

    for (int i = 0; i < 5; ++i) {
        const int textSize = windowMetrics.CalculateStringWidth(lblAttribType[i].GetWindowTextW().c_str());
        typeWidth = std::max(textSize, typeWidth);
    }

    const int polarityWidth = windowMetrics.CalculateStringWidth(btnAttribPolarity[0].GetWindowText().c_str()) +
        windowMetrics.CalculateStringWidth(btnAttribPolarity[1].GetWindowText().c_str()) +
        RadioButtonBaseWidth;

    if (polarityWidth > typeWidth) {
        typeWidth = polarityWidth;
    }

    // The Attrib width must be the longest string/buttons times 3 so everything is even
    const int totalAttribWidth = (std::max(std::max(headWidth1, headWidth2), typeWidth)) * 3;

    // Now just to figure out if any other string is wider than the above.

    int singleLineWidth = 0;

    for (int k = 0; k < 2; ++k) {

        int stringWidth = windowMetrics.CalculateStringWidth(btnEffect[k].GetWindowText().c_str());
        singleLineWidth = std::max(stringWidth + RadioButtonBaseWidth, singleLineWidth);

        stringWidth = windowMetrics.CalculateStringWidth(lblSenses[k].GetWindowText().c_str());
        singleLineWidth = std::max(stringWidth, singleLineWidth);
    }

    pageWidth = std::max(singleLineWidth, totalAttribWidth);

    pageWidth += CS.XGROUPBOX_MARGIN * 2;
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

    for (int i = 0; i < 2; ++i) {

        btnEffect[i].MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YRADIOBUTTON);

        cPos.Offset(0, CD.YRADIOBUTTON);

        if (i != 1) {
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

    for (int k = 0; k < 5; ++k) {

        lblAttribType[k].MoveWindow(cPos.x, cPos.y,
                                    firstColumnWidth, CD.YLABEL);

        if (k == 0) {

            CPoint headerPos(cPos);
            headerPos.Offset(firstColumnWidth, 0);
            lblAttribHeading[0].MoveWindow(headerPos.x, headerPos.y,
                                           columnWidth, CD.YLABEL);

            headerPos.Offset(columnWidth, 0);
            lblAttribHeading[1].MoveWindow(headerPos.x, headerPos.y,
                                           columnWidth, CD.YLABEL);
        }

        cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);
        CPoint polarPos(cPos);

        for (int l = 0; l < 2; ++l) {

            btnAttribPolarity[(k * 2) + l].MoveWindow(polarPos.x, polarPos.y,
                                                      firstColumnWidth / 2, CD.YRADIOBUTTON);

            polarPos.Offset(firstColumnWidth - (firstColumnWidth / 2), 0);

        }

        polarPos = cPos;
        polarPos.Offset(firstColumnWidth, 0);

        for (int m = 0; m < 2; ++m) {

            txtAttribAmount[(k * 2) + m].MoveWindow(polarPos.x, polarPos.y,
                                                    columnWidth, CD.YTEXTBOX_ON_BUTTON_ROW);

            spnAttribAmount[(k * 2) + m].SetBuddy(txtAttribAmount[(k * 2) + m].GetHwnd());
            polarPos.Offset(columnWidth, 0);

        }

        cPos.Offset(0, CD.YTEXTBOX_ON_BUTTON_ROW);
    }

    for (int n = 0; n < 2; n++) {

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

//=============================================================================
//
// Locations Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectLocationsTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpLocations.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("LocationsGroupLabel", grpLocations,
                                   caption, langMap);

    for (int i = 0; i < 3; ++i) {

        DWORD style = BS_AUTORADIOBUTTON | (i == 0 ? WS_GROUP : 0);
        btnLocatedAt[i].Create(*this, 0, style);
        btnLocatedAt[i].SetDlgCtrlID(LocationsTab::ControlIDs::OnGround + i);

    }

    SetWindowTextFromLangMapString("OnGroundAtLabel", btnLocatedAt[0], caption, langMap);
    SetWindowTextFromLangMapString("PlayerStartsWithLabel", btnLocatedAt[1], caption, langMap);
    SetWindowTextFromLangMapString("CharacterHasItLabel", btnLocatedAt[2], caption, langMap);

    for (int k = 0; k < 2; ++k) {
        lblGroundCoord[k].Create(*this, 0, SS_SIMPLE);
        lblDoorCoord[k].Create(*this, 0, SS_SIMPLE);
    }

    SetWindowTextFromLangMapString("XCoord", lblGroundCoord[0], caption, langMap);
    SetWindowTextFromLangMapString("YCoord", lblGroundCoord[1], caption, langMap);
    SetWindowTextFromLangMapString("XCoord", lblDoorCoord[0], caption, langMap);
    SetWindowTextFromLangMapString("YCoord", lblDoorCoord[1], caption, langMap);

    for (int k = 0; k < 2; ++k) {
        txtGroundCoord[k].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        txtGroundCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        txtGroundCoord[k].LimitText(2); // 0 to 99
        txtGroundCoord[k].EnableWindow(FALSE);
        txtGroundCoord[k].SetDlgCtrlID(LocationsTab::ControlIDs::XGroundText + k);
    }


    cbxWhichCharacter.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL | WS_TABSTOP);
    cbxWhichCharacter.SetDlgCtrlID(LocationsTab::ControlIDs::WhichCharacter);
    AddStringFromLangMap("NoCharacterSelectedOption", cbxWhichCharacter, caption, langMap);

    const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();
    const size_t numCharacters = gameCharacters.size();

    for (size_t j = 0; j < numCharacters; ++j) {
        caption = AtoW((std::to_string(gameCharacters[j].getID()) + ". ").c_str());
        caption += AtoW(gameCharacters[j].getName().c_str());
        cbxWhichCharacter.AddString(caption);
    }

    btnUnlocksDoor.Create(*this, 0, BS_AUTOCHECKBOX | WS_TABSTOP);
    SetWindowTextFromLangMapString("UnlocksDoorAtLabel", btnUnlocksDoor, caption, langMap);
    btnUnlocksDoor.SetDlgCtrlID(LocationsTab::ControlIDs::UnlocksDoor);

    for (int k = 0; k < 2; ++k) {
        txtDoorCoord[k].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        txtDoorCoord[k].SetExStyle(WS_EX_CLIENTEDGE);
        txtDoorCoord[k].LimitText(2);
        txtDoorCoord[k].SetDlgCtrlID(LocationsTab::ControlIDs::XDoorText + k);
    }

    groundCoordValidator[0] = IntegerValidator(&txtGroundCoord[0], 0, gameMap->getWidth() - 1);
    groundCoordValidator[1] = IntegerValidator(&txtGroundCoord[1], 0, gameMap->getHeight() - 1);
    doorCoordVaildator[0] = IntegerValidator(&txtDoorCoord[0], 0, gameMap->getWidth() - 1);
    doorCoordVaildator[1] = IntegerValidator(&txtDoorCoord[1], 0, gameMap->getHeight() - 1);

    return retVal;

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditObjectLocationsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectLocationsTabPage";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectLocationsTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    pageWidth = 0;

    std::vector<CWnd *> controlList;

    for (int i = 0; i < 3; ++i) {
        controlList.push_back(&btnLocatedAt[i]);
    }

    const size_t clSize = controlList.size();

    for (size_t j = 0; j < clSize; ++j) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
            controlList[j]->GetWindowTextW().c_str()),
            pageWidth);
    }

    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();
    LONG btnWidth = windowMetrics.CalculateStringWidth(btnUnlocksDoor.GetWindowTextW().c_str());
    btnWidth += CD.XCHECKBOX + CD.XCHECKBOX_GAP;

    pageWidth = std::max(btnWidth, pageWidth);

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    pageWidth += CS.XGROUPBOX_MARGIN * 2;
}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectLocationsTab::moveControls(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlSpacing CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultRadioSize(maxRowWidth, CD.YRADIOBUTTON);

    CPoint cPos(CS.XWINDOW_MARGIN + CS.XGROUPBOX_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    btnLocatedAt[0].MoveWindow(cPos.x, cPos.y,
                               defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    // In a number of dialogs, it seems that labels are bottom aligned to the text
    // boxes.

    const int lblCoordWidths = std::max(windowMetrics.CalculateStringWidth(lblDoorCoord[0].GetWindowText().c_str()) * 2,
                                        windowMetrics.CalculateStringWidth(lblDoorCoord[1].GetWindowText().c_str()) * 2);

    const DWORD bothMargins = txtDoorCoord[0].GetMargins();
    const WORD marginOffset = LOWORD(bothMargins) + HIWORD(bothMargins);

    // Even though we can only show 2 numbers, we'll make enough space for 3.
    const LONG coordWidth = windowMetrics.CalculateStringWidth(L"000");
    RECT coordRC ={ 0, 0, coordWidth + marginOffset, 0 };
    AdjustWindowRectEx(&coordRC, 0, FALSE, WS_EX_CLIENTEDGE);

    const int txtCoordWidths = coordRC.right + abs(coordRC.left);
    const int lblYOffset = (CD.YTEXTBOX_ONE_LINE_ALONE) - (CD.YLABEL);


    lblGroundCoord[0].MoveWindow(cPos.x, cPos.y + lblYOffset,
                                 lblCoordWidths, CD.YLABEL);

    txtGroundCoord[0].MoveWindow(cPos.x + lblCoordWidths, cPos.y,
                                 txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    lblGroundCoord[1].MoveWindow(cPos.x + lblCoordWidths + txtCoordWidths + CS.XRELATED_MARGIN,
                                 cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);

    txtGroundCoord[1].MoveWindow(cPos.x + (lblCoordWidths * 2) + txtCoordWidths + CS.XRELATED_MARGIN, cPos.y,
                                 txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN);

    btnLocatedAt[1].MoveWindow(cPos.x, cPos.y,
                               defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    btnLocatedAt[2].MoveWindow(cPos.x, cPos.y,
                               defaultRadioSize.cx, defaultRadioSize.cy);

    cPos.Offset(0, defaultRadioSize.cy + CS.YRELATED_MARGIN);

    cbxWhichCharacter.MoveWindow(cPos.x, cPos.y, maxRowWidth,
                                 CD.YDROPDOWN + (CD.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, CD.YCOMBOBOX + CS.YRELATED_MARGIN);

    btnUnlocksDoor.MoveWindow(cPos.x, cPos.y, maxRowWidth, CD.YCHECKBOX);

    cPos.Offset(0, CD.YCHECKBOX + CS.YRELATED_MARGIN);

    lblDoorCoord[0].MoveWindow(cPos.x, cPos.y + lblYOffset,
                               lblCoordWidths, CD.YLABEL);

    txtDoorCoord[0].MoveWindow(cPos.x + lblCoordWidths, cPos.y,
                               txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    lblDoorCoord[1].MoveWindow(cPos.x + lblCoordWidths + txtCoordWidths + CS.XRELATED_MARGIN,
                               cPos.y + lblYOffset, lblCoordWidths, CD.YLABEL);

    txtDoorCoord[1].MoveWindow(cPos.x + (lblCoordWidths * 2) + txtCoordWidths, cPos.y,
                               txtCoordWidths, CD.YTEXTBOX_ONE_LINE_ALONE);

    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE);

    grpLocations.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                            maxGroupBoxWidth, cPos.y);

    pageHeight = grpLocations.GetClientRect().Height() + CS.YUNRELATED_MARGIN;

}