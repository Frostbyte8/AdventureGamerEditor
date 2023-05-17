#include "editcharacter_dialog.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditCharacterDialog::EditCharacterDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle, bool inEditCharacter) : EditDialogBase(inMainWindow, inGameMap, inParentHandle),
descriptionsTab(NULL), attributesTab(NULL), qualitiesTab(NULL), miscTab(NULL),
isEditCharacter(inEditCharacter), optionChosen(IDCLOSE) {
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditCharacterDialog::OnClose() {

    const bool wasCanceled = optionChosen != IDOK ? true : false;   

    if(optionChosen == IDOK) {
        /*
        descriptionsTab->insertData(newCharacter);
        qualitiesTab->insertData(newCharacter);
        attributesTab->insertData(newCharacter);
        miscTab->insertData(newCharacter);
        */
    }

    ::EnableWindow(parentWindow, TRUE);
    const int alterType = isEditCharacter ? AlterType::Edit : AlterType::Add;
    CWnd::OnClose();
    mainWindow->finishedEditCharacterDialog();

}

///----------------------------------------------------------------------------
/// OnCommnad - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditCharacterDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    const WORD ctrlID = LOWORD(wParam);
    const WORD notifyCode = HIWORD(wParam);

    if(lParam) {
        if(notifyCode == BN_CLICKED) {
            if(ctrlID == IDOK) {
                 
                
                if(okClicked()) {
                    optionChosen = IDOK;
                    Close();
                }
                
                return TRUE;
                

            }
            else if(ctrlID == IDCANCEL) {
                optionChosen = IDCANCEL;
                Close();
                return TRUE;
            }
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

    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();

    tabControl.Create(*this); 

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    caption = AtoW(langMap.get(LanguageConstants::CharDescriptonsTab).c_str(), CP_UTF8);
    descriptionsTab = reinterpret_cast<EditCharacterDescriptionsTab*>(tabControl.AddTabPage(new EditCharacterDescriptionsTab(), caption));

    caption = AtoW(langMap.get(LanguageConstants::CharQualitiesTab).c_str(), CP_UTF8);
    qualitiesTab = reinterpret_cast<EditCharacterQualitiesTab*>(tabControl.AddTabPage(new EditCharacterQualitiesTab(), caption));

    caption = AtoW(langMap.get(LanguageConstants::CharAttributesTab).c_str(), CP_UTF8);
    attributesTab = reinterpret_cast<EditCharacterAttributesTab*>(tabControl.AddTabPage(new EditCharacterAttributesTab(), caption));

    caption = AtoW(langMap.get(LanguageConstants::CharMiscTab).c_str(), CP_UTF8);
    miscTab = reinterpret_cast<EditCharacterMiscTab*>(tabControl.AddTabPage(new EditCharacterMiscTab(gameMap), caption));

    std::vector<LONG> pageWidths;

    // Set the font to the font specified within window metrics.

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    descriptionsTab->calculatePageWidth(windowMetrics);
    qualitiesTab->calculatePageWidth(windowMetrics);
    attributesTab->calculatePageWidth(windowMetrics);
    miscTab->calculatePageWidth(windowMetrics);

    // The minimum dialog width, the widest tab, or the three dialog buttons
    // We will always assume 3 buttons here so the Add/Edit are consistent.

    LONG widestPoint = findLongestTab(true);
    LONG dialogButtonSize = (CD.XBUTTON * 3) + (CS.XBUTTON_MARGIN * 2) + (CS.XWINDOW_MARGIN * 2);
    widestPoint = std::max(widestPoint, dialogButtonSize);

    // We have to adjust the tab control so it's the correct dimensions first
    // and we need to use TCM_ADJUSTRECT to do so.

    RECT tabRect = {0, 0, widestPoint, 0};

    tabControl.SendMessage(TCM_ADJUSTRECT, TRUE, (LPARAM)&tabRect);

    const LONG adjustedPageWidth = tabRect.right + abs(tabRect.left);

    tabControl.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                          adjustedPageWidth, 0, FALSE);

    // TODO: For some reason we have to do this or the controls are invisible. Research why.

    tabControl.SelectPage(3);
    tabControl.SelectPage(2);
    tabControl.SelectPage(1);
    tabControl.SelectPage(0);

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
    /*
    for(int i = numDialogButtons - 1; i >= 0; --i) {
        btnDialogControl[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        cPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }
    */


    // TODO: Finish calculating dimensions
    RECT rc = {0, 0, adjustedPageWidth + (CS.XWINDOW_MARGIN * 2), 550};
    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());


    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top), SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

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


///----------------------------------------------------------------------------
/// okClicked - Called when the OK button is clicked. Checks to see if all the
/// data is valid. If it is, then the action will be successful.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool EditCharacterDialog::okClicked() {

    std::vector<ECTabViewBase*> tabPages;

    const size_t numTabs = 4;
    tabPages.reserve(numTabs);
    tabPages.push_back(descriptionsTab);
    tabPages.push_back(qualitiesTab);
    tabPages.push_back(attributesTab);
    tabPages.push_back(miscTab);

    for(size_t i = 0; i < numTabs; ++i) {
        ECTabViewBase& tabPage = *tabPages[i];
        const InputValidator* validator = tabPage.validateFields();

        if(validator != NULL) {

            CString errorMessage;
            CString errorTitle;
            processValidatorError(errorMessage, errorTitle, validator);

            MessageBox(errorMessage, L"", MB_OK | MB_ICONERROR);

            tabControl.SelectPage(i);
            if(validator->getErrorCode() != errorCodes::ControlNotFound) {
                validator->getWindow()->SetFocus();
            }
            
            return false;
        }
    }

    return true;
}
