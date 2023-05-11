#include "editobject_dialog.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditObjectDialog::EditObjectDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle, bool inEditObject) : EditDialogBase(inMainWindow, inGameMap, inParentHandle) {
    descriptionsTab = NULL;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditObjectDialog::OnClose() {
    
    ::EnableWindow(parentWindow, TRUE);
    CWnd::OnClose();
    mainWindow->finishedEditObjectDialog();
}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditObjectDialog::OnCreate(CREATESTRUCT& cs) {

    const WindowMetrics::ControlSpacing ctrlSpace = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics.GetControlDimensions();

    tabControl.Create(*this); 

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    caption = AtoW(langMap.get(LanguageConstants::DescriptionTab).c_str(), CP_UTF8);
    descriptionsTab = reinterpret_cast<EditObjectDescriptionsTab*>(tabControl.AddTabPage(new EditObjectDescriptionsTab(), caption));
    
    caption = AtoW(langMap.get(LanguageConstants::QualitiesTab).c_str(), CP_UTF8);
    qualitiesTab = reinterpret_cast<EditObjectQualitiesTab*>(tabControl.AddTabPage(new EditObjectQualitiesTab(gameMap), caption));

    caption = AtoW(langMap.get(LanguageConstants::EffectsTab).c_str(), CP_UTF8);
    effectsTab = reinterpret_cast<EditObjectEffectsTab*>(tabControl.AddTabPage(new EditObjectEffectsTab(), caption));

    caption = AtoW(langMap.get(LanguageConstants::LocationsTab).c_str(), CP_UTF8);
    locationsTab = reinterpret_cast<EditObjectLocationsTab*>(tabControl.AddTabPage(new EditObjectLocationsTab(gameMap), caption));

    for(int i = 0; i < 3; ++i) {
        btnDialogControl[i].Create(*this, 0, BS_PUSHBUTTON | WS_TABSTOP);
    }

    std::vector<LONG> pageWidths;

    // Set the font to the font specified within window metrics.

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);


    // Next, find the widest tab, and make sure that the Dialog buttons
    // are not even wider than that.

    descriptionsTab->calculatePageWidth(windowMetrics);
    qualitiesTab->calculatePageWidth(windowMetrics);
    effectsTab->calculatePageWidth(windowMetrics);
    locationsTab->calculatePageWidth(windowMetrics);

    pageWidths.push_back(descriptionsTab->getPageWidth());
    pageWidths.push_back(qualitiesTab->getPageWidth());
    pageWidths.push_back(effectsTab->getPageWidth());
    pageWidths.push_back(locationsTab->getPageWidth());

    const size_t numTabs = pageWidths.size();
    
    LONG widestTab = 0;

    for(size_t i = 0; i < numTabs; ++i) {
        widestTab = std::max(pageWidths[i], widestTab);
    }

    // TODO: Verify that this is correct.
    // The 3rd margin is for the tab control, the other 2 are for inside the groupbox, I think.

    widestTab += ((ctrlSpace.XWINDOW_MARGIN * 3) + (ctrlSpace.XGROUPBOX_MARGIN * 2));

    // Make sure that the 2/3 dialog buttons aren't wider than the tab.
    LONG dialogButtonSize = (CD.XBUTTON * 3) + (ctrlSpace.XBUTTON_MARGIN * 2) + (ctrlSpace.XWINDOW_MARGIN);
    widestTab = std::max(widestTab, dialogButtonSize);
    

    // Now that we know what our widest section is, we can resize our tab control
    // and resize the contents of the tab pages to fit.
    tabControl.MoveWindow(ctrlSpace.XWINDOW_MARGIN, ctrlSpace.YWINDOW_MARGIN, widestTab, 0, FALSE);

    tabControl.SelectPage(3);
    tabControl.SelectPage(2);
    tabControl.SelectPage(1);
    tabControl.SelectPage(0);

    descriptionsTab->moveControls(windowMetrics);
    qualitiesTab->moveControls(windowMetrics);
    effectsTab->moveControls(windowMetrics);
    locationsTab->moveControls(windowMetrics);   

    // Now we can figure out how tall the tab control needs to be.

    LONG tallestTab = effectsTab->getPageHeight() + (ctrlSpace.YWINDOW_MARGIN * 2); // This tab is always the tallest.
    tabControl.MoveWindow(ctrlSpace.XWINDOW_MARGIN, ctrlSpace.YWINDOW_MARGIN, widestTab, tallestTab, FALSE);

    // Finally, we need to move the dialog buttons into place
    CPoint cPos(ctrlSpace.XWINDOW_MARGIN + widestTab, ctrlSpace.YWINDOW_MARGIN + tallestTab + ctrlSpace.YUNRELATED_MARGIN);

    cPos.Offset(-(CD.XBUTTON), 0);

    // TODO: Only show apply when editing an existing object, not creating a new one.

    for(int i = 2; i >= 0; --i) {
        btnDialogControl[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        cPos.Offset(-(CD.XBUTTON + ctrlSpace.XBUTTON_MARGIN), 0);
    }

    btnDialogControl[0].SetDlgCtrlID(IDOK);
    btnDialogControl[1].SetDlgCtrlID(IDCANCEL);

    EOD_SetWindowText(LanguageConstants::GenericOKButtonCaption, btnDialogControl[0], caption, langMap);
    EOD_SetWindowText(LanguageConstants::GenericCancelButtonCaption, btnDialogControl[1], caption, langMap);

    btnDialogControl[0].SetStyle(btnDialogControl[0].GetStyle() | BS_DEFPUSHBUTTON);

    RECT rc = {0, 0, widestTab + (ctrlSpace.XWINDOW_MARGIN * 2),  cPos.y + CD.YBUTTON + ctrlSpace.YWINDOW_MARGIN};
    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
    
    // TODO: For some reason, the window is shown even if it's specified not to. Figure out why, or at least figure out
    // if SWP_HIDEWINDOW is necessary. Look into precreate

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top), SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

    return CWnd::OnCreate(cs);

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

    GameObject::Builder bd;
    descriptionsTab->insertData(bd);
    qualitiesTab->insertData(bd);
    effectsTab->insertData(bd);
    locationsTab->insertData(bd);
    return bd;
}