#include "editcharacter_tabviews.h"
#include "../model/gamecharacter.h"

#include <algorithm>
#include "shared_functions.h"

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterDescriptionsTab::OnCreate(CREATESTRUCT& cs) {
    return CWnd::OnCreate(cs);
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterDescriptionsTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

