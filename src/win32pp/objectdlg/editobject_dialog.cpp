#include "editobject_dialog.h"
#include "../shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditObjectDialog::EditObjectDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle, bool inEditObject) : EditDialogBase(inMainWindow, inParentHandle),
gameMap(inGameMap), descriptionsTab(0), qualitiesTab(0), effectsTab(0), locationsTab(0),
isEditObject(inEditObject) {
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getAlteredObject - Get a copy of the object that is being built by this
/// dialog window.
/// @return a Copy of a GameObject::Builder object.
///----------------------------------------------------------------------------

GameObject::Builder EditObjectDialog::getAlteredObject() {
    return newObject;
}

///----------------------------------------------------------------------------
/// isEditingObject - Returns whether this dialog is editing an object that
/// already exists, or is creating a new one.
/// @return true if it is editing an object, or false if it is creating a new
/// one.
///----------------------------------------------------------------------------

const bool EditObjectDialog::isEditingObject() const {
    return isEditObject;
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setObjectToEdit - Copy the object being edited into the dialog so the copy
/// can be worked on.
/// @param a constant reference of the desired GameObject
///----------------------------------------------------------------------------

void EditObjectDialog::setObjectToEdit(const GameObject& gameObject) {

    if(descriptionsTab) {
        newObject = GameObject::Builder(gameObject);
        descriptionsTab->populateFields(gameObject, *gameMap);
        qualitiesTab->populateFields(gameObject, *gameMap);
        effectsTab->populateFields(gameObject, *gameMap);
        locationsTab->populateFields(gameObject, *gameMap);
    }

}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditObjectDialog::OnClose() {

    // First we'll see if we can actually close the dialog.
    if(!tryClose()) {
        return;
    }

    // Then we'll end the dialog and inform the parent window
    // that we are done.
    
    endModal(&MainWindowInterface::finishedEditObjectDialog);

}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditObjectDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    const WORD ctrlID = LOWORD(wParam);
    const WORD notifyCode = HIWORD(wParam);

    if(lParam && notifyCode == BN_CLICKED) {
        if(ctrlID == IDOK || ctrlID == IDCANCEL || 
            ctrlID == DefControlIDs::IDAPPLY) {

            dialogButtonPressed(ctrlID);
            return TRUE;

        }
    }

    return FALSE;

}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditObjectDialog::OnCreate(CREATESTRUCT& createStruct) {

    const int numDialogButtons = isEditObject ? 3 : 2;
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
                   tabControl.AddTabPage(new EditObjectQualitiesTab(gameMap, parentWindow), caption));

    caption = LM_toUTF8("EffectsTabLabel", langMap);
    effectsTab = reinterpret_cast<EditObjectEffectsTab*>(
                 tabControl.AddTabPage(new EditObjectEffectsTab(parentWindow), caption));

    caption = LM_toUTF8("LocationsTabLabel", langMap);
    locationsTab = reinterpret_cast<EditObjectLocationsTab*>(
                   tabControl.AddTabPage(new EditObjectLocationsTab(gameMap, parentWindow), caption));

    // We also need to create the Ok, Cancel and Apply buttons too.

    for(int i = 0; i < numDialogButtons; ++i) {
        btnDialogControl[i].Create(*this, 0, BS_PUSHBUTTON | WS_TABSTOP);
    }

    btnDialogControl[0].SetDlgCtrlID(IDOK);
    btnDialogControl[1].SetDlgCtrlID(IDCANCEL);

    SetWindowTextFromLangMapString("OKButton",
                      btnDialogControl[0], caption, langMap);

    SetWindowTextFromLangMapString("CancelButton",
                      btnDialogControl[1], caption, langMap);

    btnDialogControl[0].SetStyle(btnDialogControl[0].GetStyle() | BS_DEFPUSHBUTTON);

    // If we are editing an object, we can also setup the apply button.
    if(isEditObject) {
        btnDialogControl[2].SetDlgCtrlID(DefControlIDs::IDAPPLY);
        btnDialogControl[2].EnableWindow(FALSE);
        SetWindowTextFromLangMapString("ApplyButton",
                          btnDialogControl[2], caption, langMap);

    }

    tabControl.SetWindowPos(btnDialogControl[numDialogButtons - 1], 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // The font must be set on the controls before we do any kind of sizing
    // so we can reliably test widths on the controls

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    // Move the controls into place, and display the first tab.

    moveControls();
    tabControl.SelectPage(0);

    return CWnd::OnCreate(createStruct);

}

LRESULT EditObjectDialog::OnNotify(WPARAM wParam, LPARAM lParam) {

    NMHDR* notifyHDR = reinterpret_cast<NMHDR*>(lParam);

    if(notifyHDR->hwndFrom == tabControl.GetHwnd() && notifyHDR->code == TCN_SELCHANGE) {
        // Update Misc Tab if necessary
        locationsTab->canBeHeldByEntities(!qualitiesTab->isFixedFlagChecked());
    }

    return 0; 
}

///----------------------------------------------------------------------------
/// onSize - Processes the WM_SIZE message for the dialog window
///----------------------------------------------------------------------------

LRESULT EditObjectDialog::onSize(WPARAM& wParam, LPARAM& lParam) {

    /*
    const WORD newWidth = LOWORD(lParam);
    const WORD newHeight = HIWORD(lParam);

    bool hasHorz = false;
    bool hasVert = false;
    int whichBars = 0;

    if(newWidth < contentSize.cx) {
        hasHorz = true;
    }

    if(newHeight < contentSize.cy) {
        hasVert = true;
    }

    // Check to see if we need a horizontal one after turning
    // on the vertical one
    if(hasHorz == false && hasVert == true) {
        if(newWidth < contentSize.cx - 17) {
            hasHorz = true;
        }

    }
    else if(hasVert == false && hasHorz == true) {
        if(newHeight < contentSize.cy - 17) {
            hasVert = true;
        }
    }

    if(hasHorz && hasVert) {
        ShowScrollBar(SB_BOTH, TRUE);
    }
    else if(hasHorz) {
        ShowScrollBar(SB_HORZ, TRUE);
        ShowScrollBar(SB_VERT, FALSE);
    }
    else if(hasVert) {
        ShowScrollBar(SB_VERT, TRUE);
        ShowScrollBar(SB_HORZ, FALSE);
    }
    else {
        ShowScrollBar(SB_BOTH, FALSE);
    }
    */

    return 0;

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
/// WndProc - Window Procedure for the Frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT EditObjectDialog::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    if(msg == WM_SIZE) {
        //return OnSize(wParam, lParam);
    }

    return WndProcDefault(msg, wParam, lParam);
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectDialog::PreTranslateMessage(MSG &msg) {

    if (IsDialogMessage(msg)) {
        return TRUE;
    }

    return CWnd::PreTranslateMessage(msg);

}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// notifyChangeMade - Change the apply button to be usable.
///----------------------------------------------------------------------------

void EditObjectDialog::notifyChangeMade() {
    if(isEditObject) {
        btnDialogControl[2].EnableWindow(TRUE);
    }
}

///----------------------------------------------------------------------------
/// notifyChangesSaved - Change the apply button to be unusable.
///----------------------------------------------------------------------------

void EditObjectDialog::notifyChangesSaved() {
    if(isEditObject) {
        btnDialogControl[2].EnableWindow(FALSE);
    }
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

    RECT tabRect = {0, 0, widestPoint, 0};

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
    for(int i = numDialogButtons - 1; i >= 0; --i) {
        btnDialogControl[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        cPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }

    // Finally, Resize the dialog window, but be careful not to move it or
    // activate it in anyway. TODO: DPI may require a call to center this,
    // so we may need monitor information of some kind.

    RECT rc = {0, 0,
               adjustedPageWidth + (CS.XWINDOW_MARGIN * 2),
               cPos.y + CD.YBUTTON + CS.YWINDOW_MARGIN };

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
    
    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);
}

///----------------------------------------------------------------------------
/// trySaveData - Confirm that data in the dialog (in this case, each tab page)
/// is valid, and if it is, save it. This function should not be called
/// directly.
/// @return true if the data was valid, false if it was not.
///----------------------------------------------------------------------------

bool EditObjectDialog::trySaveData() {

    std::vector<EOTabViewBase*> tabPages;
    
    const size_t numTabs = 4;
    tabPages.reserve(numTabs);
    tabPages.push_back(descriptionsTab);
    tabPages.push_back(qualitiesTab);
    tabPages.push_back(effectsTab);
    tabPages.push_back(locationsTab);

    // Iterate over the tabs, and validate their fields.
    for(size_t i = 0; i < numTabs; ++i) {

        EOTabViewBase& tabPage = *tabPages[i];
        const InputValidator* validatorFailed = tabPage.validateFields();

        if(validatorFailed) {

            std::string errorMessage;
            std::string errorTitle;

            processValidatorError(errorMessage, errorTitle, validatorFailed);
            displayErrorMessage(errorMessage, errorTitle);

            tabControl.SelectPage(i);

            // TODO: Return an ID instead of a HWND
            // Also, tell the tab what action to take (IE: if it's a textbox,
            // hilight it)
            if(validatorFailed->getErrorCode() != errorCodes::ControlNotFound) {
                validatorFailed->getWindow()->SetFocus();
            }
            
            return false;

        }

    }

    descriptionsTab->insertData(newObject);
    qualitiesTab->insertData(newObject);
    effectsTab->insertData(newObject);
    locationsTab->insertData(newObject);

    return true;

}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// findLongestTab - Finds the Widest/Tallest tab, and returns how long it is.
/// @param if true, find the width, if false, find the height
/// @returns the length of the tab in the given direction
///----------------------------------------------------------------------------

LONG EditObjectDialog::findLongestTab(const bool getWidth) {

    LONG longestTab = 0;
    std::vector<LONG> pageDims;

    const WindowMetrics::ControlSpacing CS = windowMetrics.GetControlSpacing();

    if(getWidth) {

        // TODO: Window metrics isn't needed?
        descriptionsTab->calculatePageWidth(windowMetrics);
        qualitiesTab->calculatePageWidth(windowMetrics);
        effectsTab->calculatePageWidth(windowMetrics);
        locationsTab->calculatePageWidth(windowMetrics);

        pageDims.push_back(descriptionsTab->getPageWidth());
        pageDims.push_back(qualitiesTab->getPageWidth());
        pageDims.push_back(effectsTab->getPageWidth());
        pageDims.push_back(locationsTab->getPageWidth());
    }
    else {

        // For height, it is calculated after all the controls are moved
        // into place.

        pageDims.push_back(descriptionsTab->getPageHeight());
        pageDims.push_back(qualitiesTab->getPageHeight());
        pageDims.push_back(effectsTab->getPageHeight());
        pageDims.push_back(locationsTab->getPageHeight());
    }

    const size_t numTabs = pageDims.size();

    for(size_t i = 0; i < numTabs; ++i) {
        longestTab = std::max(pageDims[i], longestTab);
    }

    // Apply margins.

    if(getWidth) {

        longestTab += CS.XWINDOW_MARGIN * 2;

    }
    else {
        longestTab += CS.YWINDOW_MARGIN * 2;
    }

    return longestTab;
}
