#include "editobject_dialog.h"
#include "shared_functions.h"

namespace ControlIDs {
    const WORD ID_APPLY     = 1001;
}

//=============================================================================
// Constructors
//=============================================================================

EditObjectDialog::EditObjectDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle, bool inEditObject) : EditDialogBase(inMainWindow, inGameMap, inParentHandle),
descriptionsTab(0), qualitiesTab(0), effectsTab(0), locationsTab(0), optionChosen(IDCLOSE),
isEditObject(inEditObject) {
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditObjectDialog::OnClose() {

    const bool wasCanceled = optionChosen != IDOK ? true : false;

    if(optionChosen == IDCLOSE) {
        // TODO: If changes have been made, prompt the user to ensure they
        // did not accidentally close the window.
    }

    if(optionChosen == IDOK) {
        descriptionsTab->insertData(newObject);
        qualitiesTab->insertData(newObject);
        effectsTab->insertData(newObject);
        locationsTab->insertData(newObject);
    }
    
    ::EnableWindow(parentWindow, TRUE);
    const int alterType = isEditObject ? AlterType::Edit : AlterType::Add;
    CWnd::OnClose();
    mainWindow->finishedEditObjectDialog(alterType, wasCanceled);
}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditObjectDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

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

int EditObjectDialog::OnCreate(CREATESTRUCT& createStruct) {

    const WindowMetrics::ControlSpacing CS = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();
    const int numDialogButtons = isEditObject ? 3 : 2;

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    tabControl.Create(*this); 

    // Create 4 Tabs for the Tab Control.

    caption = LM_toUTF8(LanguageConstants::DescriptionTab, langMap);
    descriptionsTab = reinterpret_cast<EditObjectDescriptionsTab*>(
                      tabControl.AddTabPage(new EditObjectDescriptionsTab(), caption));
    
    caption = LM_toUTF8(LanguageConstants::QualitiesTab, langMap);
    qualitiesTab = reinterpret_cast<EditObjectQualitiesTab*>(
                   tabControl.AddTabPage(new EditObjectQualitiesTab(gameMap), caption));

    caption = LM_toUTF8(LanguageConstants::EffectsTab, langMap);
    effectsTab = reinterpret_cast<EditObjectEffectsTab*>(
                 tabControl.AddTabPage(new EditObjectEffectsTab(), caption));

    caption = LM_toUTF8(LanguageConstants::LocationsTab, langMap);
    locationsTab = reinterpret_cast<EditObjectLocationsTab*>(
                   tabControl.AddTabPage(new EditObjectLocationsTab(gameMap), caption));

    // We also need to create the Ok, Cancel and Apply buttons too.

    for(int i = 0; i < numDialogButtons; ++i) {
        btnDialogControl[i].Create(*this, 0, BS_PUSHBUTTON | WS_TABSTOP);
    }

    btnDialogControl[0].SetDlgCtrlID(IDOK);
    btnDialogControl[1].SetDlgCtrlID(IDCANCEL);

    EOD_SetWindowText(LanguageConstants::GenericOKButtonCaption,
                      btnDialogControl[0], caption, langMap);

    EOD_SetWindowText(LanguageConstants::GenericCancelButtonCaption,
                      btnDialogControl[1], caption, langMap);

    if(isEditObject) {
        btnDialogControl[2].SetDlgCtrlID(ControlIDs::ID_APPLY);
        EOD_SetWindowText(LanguageConstants::GenericApplyButtonCaption,
                          btnDialogControl[2], caption, langMap);
    }

    btnDialogControl[0].SetStyle(btnDialogControl[0].GetStyle() | BS_DEFPUSHBUTTON);

    // The font must be set on the controls before we do any kind of sizing
    // so we can reliably test widths on the controls

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    // Now to find the widest point. We'll see what is longer:
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

    // TODO: For some reason, if this is not done, the tabs never show up.
    tabControl.SelectPage(3);
    tabControl.SelectPage(2);
    tabControl.SelectPage(1);
    tabControl.SelectPage(0);

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
    // activate it in anyway.

    RECT rc = {0, 0,
               adjustedPageWidth + (CS.XWINDOW_MARGIN * 2),
               cPos.y + CD.YBUTTON + CS.YWINDOW_MARGIN };

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
    
    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

    return CWnd::OnCreate(createStruct);

}

///----------------------------------------------------------------------------
/// OnSize - Processes the WM_SIZE message for the dialog window
///----------------------------------------------------------------------------

LRESULT EditObjectDialog::OnSize(WPARAM& wParam, LPARAM& lParam) {

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

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// getAlteredObject - Get a copy of the object that is being built by this
/// dialog window.
/// @returns a Copy of a GameObject::Builder object.
///----------------------------------------------------------------------------

GameObject::Builder EditObjectDialog::getAlteredObject() {

    return newObject;
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

///----------------------------------------------------------------------------
/// okClicked - Called when the OK button is clicked. Checks to see if all the
/// data is valid. If it is, then the action will be successful.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool EditObjectDialog::okClicked() {

    std::vector<EOTabViewBase*> tabPages;

    const size_t numTabs = 4;
    tabPages.reserve(numTabs);
    tabPages.push_back(descriptionsTab);
    tabPages.push_back(qualitiesTab);
    tabPages.push_back(effectsTab);
    tabPages.push_back(locationsTab);

    for(size_t i = 0; i < numTabs; ++i) {
        EOTabViewBase& tabPage = *tabPages[i];
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
