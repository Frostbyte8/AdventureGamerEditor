#include "editobject_tabviews.h"
#include "../shared_functions.h"
#include "../../model/gameobject.h"

#include <algorithm>

using namespace EditObjectDialogConstants;

//=============================================================================
//
// Descriptions Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectDescriptionsTab::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    SetWindowTextFromLangMapString("DescriptionsGroupLabel", grpDescriptions, caption, langMap);

    for (int i = 0; i < GameObjectDescriptions::NumAllDescriptions; i++) {

        lblDescriptions[i].Create(*this, 0, SS_SIMPLE);
        txtDescriptions[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
        txtDescriptions[i].SetExStyle(WS_EX_CLIENTEDGE);

        if (i == 0) {
            txtDescriptions[i].LimitText(GameObjectConstants::MaxNameLength);
        } else {
            txtDescriptions[i].LimitText(GameObjectConstants::MaxDescriptionLength);
        }

        // For the last two fields, we need browse buttons
        if (i > GameObjectDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameObjectDescriptions::NumDescriptions;

            btnBrowse[currentButton].Create(*this, 0,
                                            WS_TABSTOP | BS_PUSHBUTTON);
            SetWindowTextFromLangMapString("BrowseButton", btnBrowse[currentButton], caption, langMap);
        } else {
            txtDescriptions[i].SetDlgCtrlID(DescriptionsTab::ControlIDs::Name + i);
        }

    }

    SetWindowTextFromLangMapString("ObjectNameLabel", lblDescriptions[0], caption, langMap);
    SetWindowTextFromLangMapString("OnObjectSightLabel", lblDescriptions[1], caption, langMap);
    SetWindowTextFromLangMapString("OnUseLabel", lblDescriptions[2], caption, langMap);
    SetWindowTextFromLangMapString("OnLastUseLabel", lblDescriptions[3], caption, langMap);
    SetWindowTextFromLangMapString("ObjectIconLabel", lblDescriptions[4], caption, langMap);
    SetWindowTextFromLangMapString("ObjectSoundLabel", lblDescriptions[5], caption, langMap);

    txtDescriptions[GameObjectDescriptions::Icon].EnableWindow(FALSE);
    txtDescriptions[GameObjectDescriptions::Sound].EnableWindow(FALSE);

    btnBrowse[0].SetDlgCtrlID(DescriptionsTab::ControlIDs::BrowseIcon);
    btnBrowse[1].SetDlgCtrlID(DescriptionsTab::ControlIDs::BrowseSound);

    std::vector<std::string> imageExtensions;
    imageExtensions.push_back(".ICO");
    imageExtensions.push_back(".BMP");

    iconDescValidator = StringValidator(&txtDescriptions[4], NULL, &imageExtensions, 12, 5);
    soundDescValidator = StringValidator(&txtDescriptions[5], "", ".WAV", 12, 5);

    return retVal;

}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectDescriptionsTabPage";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::calculatePageWidth(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();

    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for (int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
            lblDescriptions[i].GetWindowTextW().c_str()), pageWidth);
    }

    const WindowMetrics::ControlSpacing cs = windowMetrics.GetControlSpacing();
    pageWidth += cs.XGROUPBOX_MARGIN * 2;

}

///----------------------------------------------------------------------------
/// moveControls - Move/resize the controls to their desired positions
/// @param a constant reference to the dialog's window metrics object to be used
/// to move and size the controls.
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::moveControls(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultEditSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN,
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    for (int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {

        lblDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

        if (i > GameObjectDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameObjectDescriptions::NumDescriptions;

            btnBrowse[currentButton].MoveWindow(cPos.x, cPos.y,
                                                CD.XBUTTON, CD.YBUTTON);

            if (i != 5) {
                cPos.Offset(0, CD.YBUTTON + CS.YRELATED_MARGIN);
            } else {
                cPos.Offset(0, CD.YBUTTON);
            }

        }

    }

    grpDescriptions.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                               maxGroupBoxWidth, cPos.y);

    pageHeight = cPos.y + CS.YWINDOW_MARGIN + CS.YUNRELATED_MARGIN;

}