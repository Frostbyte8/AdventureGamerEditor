#include "editcharacter_dialog.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditCharacterDialog::EditCharacterDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle, bool inEditCharacter) : EditDialogBase(inMainWindow, inParentHandle),
gameMap(inGameMap), descriptionsTab(NULL), attributesTab(NULL), qualitiesTab(NULL), miscTab(NULL),
isEditCharacter(inEditCharacter) {
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getAlteredCharacter - Get a copy of the object that is being built by this
/// dialog window.
/// @return a Copy of a GameCharacter::Builder object.
///----------------------------------------------------------------------------

GameCharacter::Builder EditCharacterDialog::getAlteredCharacter() {
    return newCharacter;
}

///----------------------------------------------------------------------------
/// isEditingCharacter - Checks if the dialog is editing a character that
/// exists.
/// @return try if it is editing a character, false if it's creating one
///----------------------------------------------------------------------------

const bool EditCharacterDialog::isEditingCharacter() const {
    return isEditCharacter;
}

//=============================================================================
// Mutators
//=============================================================================

void EditCharacterDialog::setCharacterToEdit(const GameCharacter& gameCharacter) {
    
    if(descriptionsTab) {

        newCharacter = GameCharacter::Builder(gameCharacter);

        descriptionsTab->populateFields(gameCharacter);
        qualitiesTab->populateFields(gameCharacter);
        attributesTab->populateFields(gameCharacter);
        miscTab->populateFields(gameCharacter);

    }
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditCharacterDialog::OnClose() {

    if(!tryClose()) {
        return;
    }
    
    endModal(&MainWindowInterface::finishedEditCharacterDialog);

}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditCharacterDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

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

int EditCharacterDialog::OnCreate(CREATESTRUCT& cs) {

    const int numDialogButtons = isEditCharacter ? 3 : 2;
    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    tabControl.Create(*this); 

    // Create the tab pages for the dialog

    EditDialogBase* parentWindow = reinterpret_cast<EditDialogBase*>(this);

    caption = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);
    descriptionsTab = reinterpret_cast<EditCharacterDescriptionsTab*>(
                      tabControl.AddTabPage(new EditCharacterDescriptionsTab(parentWindow), caption));

    caption = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);
    qualitiesTab = reinterpret_cast<EditCharacterQualitiesTab*>(
                   tabControl.AddTabPage(new EditCharacterQualitiesTab(gameMap, parentWindow), caption));

    caption = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);
    attributesTab = reinterpret_cast<EditCharacterAttributesTab*>(
                    tabControl.AddTabPage(new EditCharacterAttributesTab(parentWindow), caption));

    caption = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);
    miscTab = reinterpret_cast<EditCharacterMiscTab*>(
              tabControl.AddTabPage(new EditCharacterMiscTab(gameMap, parentWindow), caption));

    // We also need to create the Ok, Cancel and Apply buttons too.

    for(int i = 0; i < numDialogButtons; ++i) {
        btnDialogControl[i].Create(*this, 0, BS_PUSHBUTTON | WS_TABSTOP);
    }

    btnDialogControl[0].SetDlgCtrlID(IDOK);
    btnDialogControl[1].SetDlgCtrlID(IDCANCEL);

    EOD_SetWindowText("FileMenu",
                      btnDialogControl[0], caption, langMap);

    EOD_SetWindowText("FileMenu",
                      btnDialogControl[1], caption, langMap);

    btnDialogControl[0].SetStyle(btnDialogControl[0].GetStyle() | BS_DEFPUSHBUTTON);

    // If we have an apply button, do that here too.

    if(isEditCharacter) {
        btnDialogControl[2].SetDlgCtrlID(DefControlIDs::IDAPPLY);
        EOD_SetWindowText("FileMenu",
                          btnDialogControl[2], caption, langMap);
    }

    // Set the font to the font specified within window metrics.

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    moveControls();
    tabControl.SelectPage(0);

    return CWnd::OnCreate(cs);
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the Frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT EditCharacterDialog::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
    return WndProcDefault(msg, wParam, lParam);
}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// notifyChangeMade - Change the apply button to be useable.
///----------------------------------------------------------------------------

void EditCharacterDialog::notifyChangeMade() {
    if(isEditCharacter) {
        btnDialogControl[2].EnableWindow(TRUE);
    }
}

///----------------------------------------------------------------------------
/// notifyChangesSaved - Change the apply button to be unusable.
///----------------------------------------------------------------------------

void EditCharacterDialog::notifyChangesSaved() {
    if(isEditCharacter) {
        btnDialogControl[2].EnableWindow(FALSE);
    }
}

///----------------------------------------------------------------------------
/// moveControls 
/// dialog window.
/// @returns a Copy of a GameObject::Builder object.
///----------------------------------------------------------------------------

void EditCharacterDialog::moveControls() {

    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();
    const int numDialogButtons = isEditCharacter ? 3 : 2;

    // Now to find the widest point. We'll see what is longer:
    // The minimum dialog width, or the widest tab. 4 Buttons + Spacing seems like a good
    // amount of space.

    LONG widestPoint = findLongestTab(true);
    const LONG minSize = (CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3) + (CS.XBUTTON_MARGIN * 2);
    widestPoint = std::max(widestPoint, minSize);

    // We have to adjust the tab control so it's the correct dimensions first
    // and we need to use TCM_ADJUSTRECT to do so.

    RECT tabRect = {0, 0, widestPoint, 0};

    tabControl.SendMessage(TCM_ADJUSTRECT, TRUE, (LPARAM)&tabRect);

    const LONG adjustedPageWidth = tabRect.right + abs(tabRect.left);

    tabControl.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                          adjustedPageWidth, 0, FALSE);

    // The tab pages won't have the correct width, so we need to change this
    // or everything will appear to be invisible.

    descriptionsTab->MoveWindow(0, 0, adjustedPageWidth, 0, FALSE);
    qualitiesTab->MoveWindow(0, 0, adjustedPageWidth, 0, FALSE);
    attributesTab->MoveWindow(0, 0, adjustedPageWidth, 0, FALSE);
    miscTab->MoveWindow(0, 0, adjustedPageWidth, 0, FALSE);

    // Now move the controls so they fit their new width.

    descriptionsTab->moveControls(windowMetrics);
    qualitiesTab->moveControls(windowMetrics);
    attributesTab->moveControls(windowMetrics);
    miscTab->moveControls(windowMetrics);

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
/// is valid, and if it is, save it.
/// @return true if the data was valid, false if it was not.
///----------------------------------------------------------------------------

bool EditCharacterDialog::trySaveData() {

    std::vector<ECTabViewBase*> tabPages;

    const size_t numTabs = 4;
    tabPages.reserve(numTabs);
    tabPages.push_back(descriptionsTab);
    tabPages.push_back(qualitiesTab);
    tabPages.push_back(attributesTab);
    tabPages.push_back(miscTab);

    for(size_t i = 0; i < numTabs; ++i) {

        ECTabViewBase& tabPage = *tabPages[i];
        const InputValidator* validatorFailed = tabPage.validateFields();

        if(validatorFailed) {

            std::string     errorMessage;
            std::string     errorTitle;
            processValidatorError(errorMessage, errorTitle, validatorFailed);
            displayErrorMessage(errorMessage, errorTitle);

            tabControl.SelectPage(i);

            // TODO: see EditObjectDialog's TODO
            if(validatorFailed->getErrorCode() != errorCodes::ControlNotFound) {
                validatorFailed->getWindow()->SetFocus();
            }
            
            return false;

        }
    }

    descriptionsTab->insertData(newCharacter);
    qualitiesTab ->insertData(newCharacter);
    attributesTab->insertData(newCharacter);
    miscTab->insertData(newCharacter);

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

LONG EditCharacterDialog::findLongestTab(const bool getWidth) {

    LONG longestTab = 0;
    std::vector<LONG> pageDims;

    const WindowMetrics::ControlSpacing CS = windowMetrics.GetControlSpacing();

    if(getWidth) {

        // TODO: Window metrics isn't needed?
        descriptionsTab->calculatePageWidth(windowMetrics);
        qualitiesTab->calculatePageWidth(windowMetrics);
        attributesTab->calculatePageWidth(windowMetrics);
        miscTab->calculatePageWidth(windowMetrics);

        pageDims.push_back(descriptionsTab->getPageWidth());
        pageDims.push_back(qualitiesTab->getPageWidth());
        pageDims.push_back(attributesTab->getPageWidth());
        pageDims.push_back(miscTab->getPageWidth());
    }
    else {

        // For height, it is calculated after all the controls are moved
        // into place.

        pageDims.push_back(descriptionsTab->getPageHeight());
        pageDims.push_back(qualitiesTab->getPageHeight());
        pageDims.push_back(attributesTab->getPageHeight());
        pageDims.push_back(miscTab->getPageHeight());
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