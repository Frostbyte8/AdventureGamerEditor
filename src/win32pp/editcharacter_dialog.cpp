#include "editcharacter_dialog.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditCharacterDialog::EditCharacterDialog(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics,
                    const GameMap* inGameMap, bool inEditCharacter) : mainWindow(inMainWindow),
                    windowMetrics(inWindowMetrics), gameMap(inGameMap) {
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterDialog::OnCreate(CREATESTRUCT& cs) {

    const WindowMetrics::ControlSpacing     CS = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions  CD = windowMetrics->GetControlDimensions();

    tabControl.Create(*this); 

    descriptionsTab = reinterpret_cast<EditCharacterDescriptionsTab*>(tabControl.AddTabPage(new EditCharacterDescriptionsTab(), L"Desc"));

    std::vector<LONG> pageWidths;

    descriptionsTab->calculatePageWidth(*windowMetrics);
    pageWidths.push_back(descriptionsTab->getPageWidth());

    const size_t numTabs = pageWidths.size();
    
    LONG widestTab = 0;

    for(size_t i = 0; i < numTabs; ++i) {
        widestTab = std::max(pageWidths[i], widestTab);
    }

    widestTab += ((CS.XWINDOW_MARGIN * 3) + (CS.XGROUPBOX_MARGIN * 2));

    tabControl.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN, widestTab, 500, FALSE);
    tabControl.SelectPage(0);

    descriptionsTab->moveControls(*windowMetrics);

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
