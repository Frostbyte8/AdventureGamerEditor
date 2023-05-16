#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include <algorithm>
#include <wxx_commondlg.h>

#include "shared_functions.h"

namespace ControlIDs {
    const WORD BrowseIcon       = 101;
    const WORD BrowseSound      = 102;
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditObjectDescriptionsTab::OnCommand(WPARAM wParam, LPARAM lParam) {
    
    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD ctrlAction = HIWORD(wParam);

        if(ctrlAction == BN_CLICKED && 
          (ctrlID == ControlIDs::BrowseIcon || ctrlID == ControlIDs::BrowseSound)) {
                return onBrowseForMedia(ctrlID == ControlIDs::BrowseIcon ? true : false);
        }

    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectDescriptionsTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    grpDescriptions.SetWindowTextW(L"Descriptions");

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; i++) {
        
        lblDescriptions[i].Create(*this, 0, SS_SIMPLE);
        txtDescriptions[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
        txtDescriptions[i].SetExStyle(WS_EX_CLIENTEDGE);

        if(i == 0) {
            txtDescriptions[i].LimitText(GameObjectConstants::MaxNameLength);
        }
        else {
            txtDescriptions[i].LimitText(GameObjectConstants::MaxDescriptionLength);
        }
        
        EOD_SetWindowText(LanguageConstants::NameLabel+i, lblDescriptions[i],
                          caption, langMap);

        // For the last two fields, we need browse buttons
        if(i > GameObjectDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameObjectDescriptions::NumDescriptions;

            btnBrowse[currentButton].Create(*this, 0,
                                            WS_TABSTOP | BS_PUSHBUTTON);

            EOD_SetWindowText(LanguageConstants::BrowseButton,
                              btnBrowse[currentButton], caption, langMap);
        }

    }

    txtDescriptions[GameObjectDescriptions::Icon].EnableWindow(FALSE);
    txtDescriptions[GameObjectDescriptions::Sound].EnableWindow(FALSE);

    btnBrowse[0].SetDlgCtrlID(ControlIDs::BrowseIcon);
    btnBrowse[1].SetDlgCtrlID(ControlIDs::BrowseSound);

    descValidator = StringValidator(&txtDescriptions[4], "", ".ICO", 12);

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
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectDescriptionsTab::PreTranslateMessage(MSG &msg) {

    if(IsDialogMessage(msg)) {
        return TRUE;
    }
    
    return CWnd::PreTranslateMessage(msg);

}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::calculatePageWidth(const WindowMetrics& windowMetrics) {
    
    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();

    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             lblDescriptions[i].GetWindowTextW().c_str()), pageWidth);
    }

    const WindowMetrics::ControlSpacing cs = windowMetrics.GetControlSpacing();
    pageWidth += cs.XGROUPBOX_MARGIN * 2;
    
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::insertData(GameObject::Builder& builder) {

    // TODO: For the controller version of this, send the text and let the controller
    // do the string capping.

    CString wideDesc = txtDescriptions[0].GetWindowText().Left(GameObjectConstants::MaxNameLength);
    builder.description(WtoA(wideDesc).c_str(), 0);  

    for(int i = 1; i < 6; ++i) {
        wideDesc = txtDescriptions[i].GetWindowText().Left(GameObjectConstants::MaxDescriptionLength);
        builder.description(WtoA(wideDesc).c_str(), i);
    }

}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
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

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {

        lblDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

        if(i > GameObjectDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameObjectDescriptions::NumDescriptions;

            btnBrowse[currentButton].MoveWindow(cPos.x, cPos.y, 
                                                CD.XBUTTON, CD.YBUTTON);

            if(i != 5) {
                cPos.Offset(0, CD.YBUTTON + CS.YRELATED_MARGIN);
            }
            else {
                cPos.Offset(0, CD.YBUTTON);
            }

        }

    }

    grpDescriptions.MoveWindow(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN,
                               maxGroupBoxWidth, cPos.y);


    pageHeight = grpDescriptions.GetClientRect().Height() + CS.YUNRELATED_MARGIN;

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::populateFields(const GameObject &gameObject, const GameMap& gameMap) {

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {

        CString caption;

        EOD_SetWindowText(gameObject.getDescription(i),
                          txtDescriptions[i], caption);
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditObjectDescriptionsTab::validateFields() {
   
    if(!descValidator.validate()) {
        return &descValidator;
    }

    /*
    if(txtDescriptions[4].GetWindowTextLength() > 12 ) {
        MessageBox(L"File names cannot exceed 8 characters plus 3 for the extension", L"Validation Error", MB_OK | MB_ICONERROR);
        return ControlIDs::BrowseIcon;
    }
    
    if(txtDescriptions[5].GetWindowTextLength() > 12 ) {
        MessageBox(L"File names cannot exceed 8 characters plus 3 for the extension", L"Validation Error", MB_OK | MB_ICONERROR);
        return ControlIDs::BrowseIcon;
    }
    
    CString field = txtDescriptions[4].GetWindowText().Right(4);
    field.MakeUpper();

    if(field.Compare(L".ICO") && field.Compare(L".BMP")) {
        MessageBox(L"Image must be an .ICO or a .BMP.", L"Validation Error", MB_OK | MB_ICONERROR);
        return ControlIDs::BrowseIcon;
    }

    field = txtDescriptions[5].GetWindowText().Right(4);
    field.MakeUpper();

    if(field.Compare(L".WAV")) {
        MessageBox(L"Sound file must be a .WAV", L"Validation Error", MB_OK | MB_ICONERROR);
        return ControlIDs::BrowseSound;
    }
    */

    return NULL;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// onBrowseForMedia - When the Browse for Icon or Sound icon is pressed,
/// search for one.
/// @param if true, it will search for an icon, otherwise, search for a sound.
///----------------------------------------------------------------------------

BOOL EditObjectDescriptionsTab::onBrowseForMedia(const bool findIcon) {

	CFileDialog fileDialog(TRUE, NULL, NULL, OFN_NOLONGNAMES | OFN_FILEMUSTEXIST, NULL);

    if(findIcon) {
        fileDialog.SetFilter(L"Image Files (*.BMP;*.ICO)|*.BMP;*.ICO");
        fileDialog.SetTitle(L"Find Image File");
    }
    else {
        fileDialog.SetFilter(L"Sound Files (*.WAV)|*.WAV");
        fileDialog.SetTitle(L"Find Sound File");
    }

	if(fileDialog.DoModal(GetParent().GetHwnd()) == IDOK) {

        // Convert the Long Path Name of the file into a short one. We can't use long paths
        // As the game was written for Windows 3.1.

        CString fileName;
        const long strLength = GetShortPathName(fileDialog.GetPathName(), NULL, 0);
        GetShortPathName(fileDialog.GetPathName(), fileName.GetBuffer(strLength), strLength + 1);
        fileName.ReleaseBuffer();

        const int lastSlash = fileName.ReverseFind(L"\\") + 1;
        if(lastSlash < 0) {
            MessageBox(L"Could not turn long path name into a short path name.", L"File Path Error", MB_OK | MB_ICONERROR);
            return TRUE;
        }

        fileName = fileName.Mid(lastSlash, fileName.GetLength() - lastSlash); 

        txtDescriptions[(findIcon ? 4 : 5)].SetWindowText(fileName);
	}

    return TRUE;

}
