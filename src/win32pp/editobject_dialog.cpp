#include "editobject_dialog.h"
#include "shared_functions.h"


//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditObjectDialog::OnClose() {
    
    CWnd::OnClose();

}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditObjectDialog::OnCreate(CREATESTRUCT& cs) {

    const WindowMetrics::ControlSpacing ctrlSpace = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics->GetControlDimensions();

    tabControl.Create(*this);
    descriptionsTab = reinterpret_cast<EditObjectDescriptionsTab*>(tabControl.AddTabPage(new EditObjectDescriptionsTab(windowMetrics), L"Descriptions"));
    qualitiesTab = reinterpret_cast<EditObjectQualitiesTab*>(tabControl.AddTabPage(new EditObjectQualitiesTab(windowMetrics, gameMap), L"Qualities"));
    effectsTab = reinterpret_cast<EditObjectEffectsTab*>(tabControl.AddTabPage(new EditObjectEffectsTab(windowMetrics), L"Effects"));
    locationsTab = reinterpret_cast<EditObjectLocationsTab*>(tabControl.AddTabPage(new EditObjectLocationsTab(windowMetrics, gameMap), L"Locations"));

    std::vector<LONG> pageWidths;

    //
    // TEST DATA
    //

    GameObject::Builder bd;
    bd.description("Thing", 0);
    bd.description("Looks pretty cool", 1);
    bd.description("You hold it high above your head. Nothing Happens.", 2);
    bd.description("The object for no reason at all ceases to exist suddenly.", 3);
    bd.description("thing.ico", 4);
    bd.description("hello.wav", 5);
    bd.attributeBase(-1, 0);
    bd.attributeRandom(-2, 0);
    bd.attributeBase(3, 1);
    bd.attributeRandom(4, 1);

    bd.flags1(64);
    bd.flags2(1);

    bd.location();
    bd.monetaryWorth(1234);
    bd.uses(123);
    bd.usedWithID(2);

    bd.makesHearing(HearingTypes::Normal());
    bd.makesSight(SightTypes::Blind());

    // TODO: I'm not sure why, but until you do this, the tabs are invisible. It might be because of
    // how the window is created so it might go away after this window is an actual modal window.

    //
    // TEST DATA END
    //


    // TODO: Figure out tab width
    HFONT dialogFont = windowMetrics->GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetFontTest), (LPARAM)dialogFont);

    descriptionsTab->calculatePageWidth();
    qualitiesTab->calculatePageWidth();
    effectsTab->calculatePageWidth();
    locationsTab->calculatePageWidth();

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

    LONG dialogButtonSize = (CD.XBUTTON * 3) + (ctrlSpace.XBUTTON_MARGIN * 2) + (ctrlSpace.XWINDOW_MARGIN);

    widestTab = std::max(widestTab, dialogButtonSize);
    
    tabControl.MoveWindow(ctrlSpace.XWINDOW_MARGIN, ctrlSpace.YWINDOW_MARGIN, widestTab, 0, FALSE);

    tabControl.SelectPage(3);
    tabControl.SelectPage(2);
    tabControl.SelectPage(1);
    tabControl.SelectPage(0);

    descriptionsTab->moveControls();
    qualitiesTab->moveControls();
    effectsTab->moveControls();
    locationsTab->moveControls();   
    
    LONG tallestTab = effectsTab->getPageHeight() + (ctrlSpace.YWINDOW_MARGIN * 2); // This tab is always the tallest.
    tabControl.MoveWindow(ctrlSpace.XWINDOW_MARGIN, ctrlSpace.YWINDOW_MARGIN, widestTab, tallestTab, FALSE);

    CPoint cPos(ctrlSpace.XWINDOW_MARGIN + widestTab, ctrlSpace.YWINDOW_MARGIN + tallestTab + ctrlSpace.YUNRELATED_MARGIN);

    cPos.Offset(-(CD.XBUTTON), 0);

    // TODO: Only show apply when editing an existing object, not creating a new one.

    for(int i = 2; i >= 0; --i) {
        
        btnDialogControl[i].Create(*this, 0, BS_PUSHBUTTON | WS_TABSTOP);
        btnDialogControl[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        btnDialogControl[i].SetWindowTextW(L"TEST");
        cPos.Offset(-(CD.XBUTTON + ctrlSpace.XBUTTON_MARGIN), 0);
    }

    btnDialogControl[0].SetDlgCtrlID(IDOK);
    btnDialogControl[1].SetDlgCtrlID(IDCANCEL);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    EOD_SetWindowText(LanguageConstants::GenericOKButtonCaption, btnDialogControl[0], caption, langMap);
    EOD_SetWindowText(LanguageConstants::GenericCancelButtonCaption, btnDialogControl[1], caption, langMap);

    btnDialogControl[0].SetStyle(btnDialogControl[0].GetStyle() | BS_DEFPUSHBUTTON);

    //btnDialogControl[2].SetDlgCtrlID(101);

    descriptionsTab->populateFields(bd.build());
    qualitiesTab->populateFields(bd.build());
    effectsTab->populateFields(bd.build());
    locationsTab->populateFields(bd.build());

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
/// PreRegisterClass - Override defaults for tab page
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

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// SetProperFont - Sets the font of the window and all it's child controls
/// to the font specified. Meant to be used with EnumChildWindows.
/// @param Handle to the the control whose font is to be changed
/// @param LPARAM of the font to be set on the control.
///----------------------------------------------------------------------------

bool CALLBACK EditObjectDialog::SetFontTest(HWND child, LPARAM font) {
    ::SendMessage(child, WM_SETFONT, font, true);
    return true;
}
