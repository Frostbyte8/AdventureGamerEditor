#include "edit_worldinfo.h"

namespace ControlIDs {
    const WORD WorldName           = 101;
    const WORD CurrencyName        = 102;
    const WORD txtEnergy           = 103;
    const WORD txtSkill            = 104;
    const WORD txtWillpower        = 105;
    const WORD txtLuck             = 106;
    const WORD txtTorchlife        = 107;
    const WORD spnEnergy           = 108;
    const WORD spnSkill            = 109;
    const WORD spnWillpower        = 110;
    const WORD spnLuck             = 111;
    const WORD spnTorchlife        = 112;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditWorldInfoDialog::OnCreate(CREATESTRUCT& cs) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpWorldInfo.Create(*this, 0 BS_GROUPBOX);
    EOD_SetWindowText(LanguageConstants::WorldSettingsGroup, grpWorldInfo, caption, langMap);

    for(int i = 0; i < 2; ++i) {
        lblProperties[i].Create(*this, 0, SS_SIMPLE);
        txtProperties[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
        txtProperties[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtProperties[i].LimitText(128);
        txtProperties[i].SetDlgCtrlID(ControlIDs::WorldName+i);
    }

    for(int i = 0; i < 5; ++i) {
        lblAttributes[i].Create(*this, 0, SS_SIMPLE);
        EOD_SetWindowText(LangaugeConstants::PlayerEnergy+i, lblAttributes[i], caption, langMap);
        txtAttributes[i].Create(*this, 0, ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP);
        spnAttributes[i].Create(*this, 0 WS_VISIBLE | UDS_AUTOBUDDY |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);

        txtAttributes[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtAttributes[i].LimitText(2);

        spnAttributes[i].SetRange(GameObjectConstants::MinAttributeValue,
                                  GameObjectConstants::MaxAttributeValue);

        txtAttributes[i].SetDlgCtrlID(ControlIDs::txtEnergy+i);
        spnAttributes[i].SetDlgCtrlID(ControlIDs::spnEnergy+i);
    }

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditWorldInfoDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditWorldInfoDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

void EditWorldInfoDialog::calculatePageWidth(const WindowMetrics& wndMetrics) {

    const WindowMetrics::ControlSpacing CS      = wndMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = wndMetrics.GetControlDimensions();

    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for(int i = 0; i < 2; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             lblProperties[i].GetWindowTextW().c_str()), pageWidth);
    }

    for(int i = 0; i < 5; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             lblAttributes[i].GetWindowTextW().c_str()), pageWidth);
    }

    pageWidth += CS.XGROUPBOX_MARGIN * 2;

    pageWidth = std::max(pageWidth, (CD.YBUTTON * 3) + (CS.XBUTTON_MARGIN));
}

//=============================================================================
// Private Functions
//=============================================================================

void EditWorldInfoDialog::moveControls(const WindowMetrics& wndMetrics) {

    const WindowMetrics::ControlSpacing CS      = wndMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = wndMetrics.GetControlDimensions();

    // The max width of a row is the size of the window, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultEditSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);
    
    for(int i = 0; i < 2; ++i) {

        lblProperties[i].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtProperties[i].MoveWindow(cPos.x, cPos.y,
                                    defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

    }

    for(int i = 0; i < 5; ++i) {

        lblAttributes[i].MoveWindow(cPos.x, cPos.y, defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtAttributes[i].MoveWindow(cPos.x, cPos.y,
                                    defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

        spnAttributes[i].SetBuddy(txtAttributes[i].GetHwnd());
    }

}
