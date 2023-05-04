#include "editobject_dialog.h"

void EditObjectDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

int EditObjectDialog::OnCreate(CREATESTRUCT& cs) {

    const WindowMetrics::ControlSpacing ctrlSpace = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions CD = windowMetrics->GetControlDimensions();

    tabControl.Create(*this);
    descriptionsTab = reinterpret_cast<EditObjectDescriptionsTab*>(tabControl.AddTabPage(new EditObjectDescriptionsTab(windowMetrics), L"Descriptions"));
    qualitiesTab = reinterpret_cast<EditObjectQualitiesTab*>(tabControl.AddTabPage(new EditObjectQualitiesTab(windowMetrics), L"Qualities"));
    effectsTab = reinterpret_cast<EditObjectEffectsTab*>(tabControl.AddTabPage(new EditObjectEffectsTab(windowMetrics), L"Effects"));
    locationsTab = reinterpret_cast<EditObjectLocationsTab*>(tabControl.AddTabPage(new EditObjectLocationsTab(windowMetrics), L"Locations"));

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

    // TODO: OK, CANCEL, APPLY buttons

    descriptionsTab->populateFields(bd.build());
    qualitiesTab->populateFields(bd.build());
    effectsTab->populateFields(bd.build());
    locationsTab->populateFields(bd.build());

    return CWnd::OnCreate(cs);
}

LRESULT EditObjectDialog::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    if(msg == WM_SIZE) {
        return OnSize(wParam, lParam);
    }

    return WndProcDefault(msg, wParam, lParam);
}

bool CALLBACK EditObjectDialog::SetFontTest(HWND child, LPARAM font) {
    ::SendMessage(child, WM_SETFONT, font, true);
    return true;
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

//
//
//

void EditObjectDialog::OnClose() {

    WORD retVal = qualitiesTab->validateFields(); 
    getGameObjectBuilder();

    if(retVal == 0) {
        CWnd::OnClose();
    }

    
}

GameObject::Builder EditObjectDialog::getGameObjectBuilder() {

    GameObject::Builder bd;
    descriptionsTab->insertData(bd);
    qualitiesTab->insertData(bd);
    effectsTab->insertData(bd);
    locationsTab->insertData(bd);
    return bd;
}
