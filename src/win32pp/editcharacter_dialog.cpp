#include "editcharacter_dialog.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditCharacterDialog::EditCharacterDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle, bool inEditCharacter) : EditDialogBase(inMainWindow, inGameMap, inParentHandle) {
    contentSize.SetSize(0, 0);
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditCharacterDialog::OnClose() {
    
    ::EnableWindow(parentWindow, TRUE);
    CWnd::OnClose();
    mainWindow->finishedEditCharacterDialog();
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
    miscTab = reinterpret_cast<EditCharacterMiscTab*>(tabControl.AddTabPage(new EditCharacterMiscTab(), caption));

    std::vector<LONG> pageWidths;

    // Set the font to the font specified within window metrics.

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    descriptionsTab->calculatePageWidth(windowMetrics);
    qualitiesTab->calculatePageWidth(windowMetrics);
    attributesTab->calculatePageWidth(windowMetrics);
    miscTab->calculatePageWidth(windowMetrics);

    pageWidths.push_back(descriptionsTab->getPageWidth());
    pageWidths.push_back(qualitiesTab->getPageWidth());
    pageWidths.push_back(attributesTab->getPageWidth());
    pageWidths.push_back(miscTab->getPageWidth());

    const size_t numTabs = pageWidths.size();
    
    LONG widestTab = 0;

    for(size_t i = 0; i < numTabs; ++i) {
        widestTab = std::max(pageWidths[i], widestTab);
    }

    widestTab += ((CS.XWINDOW_MARGIN * 3) + (CS.XGROUPBOX_MARGIN * 2));

    tabControl.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN, widestTab, 500, FALSE);

    tabControl.SelectPage(3);
    tabControl.SelectPage(2);
    tabControl.SelectPage(1);
    tabControl.SelectPage(0);

    descriptionsTab->moveControls(windowMetrics);
    qualitiesTab->moveControls(windowMetrics);
    attributesTab->moveControls(windowMetrics);
    miscTab->moveControls(windowMetrics);

    contentSize.SetSize(widestTab + (CS.XWINDOW_MARGIN * 2), 550);

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
/// SetProperFont - Sets the font of the window and all it's child controls
/// to the font specified. Meant to be used with EnumChildWindows.
/// @param Handle to the the control whose font is to be changed
/// @param LPARAM of the font to be set on the control.
///----------------------------------------------------------------------------

/*
bool CALLBACK EditCharacterDialog::SetProperFont(HWND child, LPARAM font) {
    ::SendMessage(child, WM_SETFONT, font, true);
    return true;
}
*/