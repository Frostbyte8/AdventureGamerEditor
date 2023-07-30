#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include <algorithm>
#include <wxx_commondlg.h>

#include "shared_functions.h"

namespace ControlIDs {
    const WORD BrowseIcon       = 101;
    const WORD BrowseSound      = 102;
    const WORD Name             = 103;
    const WORD Sight            = 104;
    const WORD OnUse            = 105;
    const WORD OnLastUse        = 106;
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

        if(ctrlID >= ControlIDs::Name && ctrlID <= ControlIDs::OnLastUse) {
            if(ctrlAction == EN_CHANGE) {
                parentWindow->madeChange();
            }
        }
        else if(ctrlAction == BN_CLICKED && 
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
    SetWindowTextFromLangMapString("DescriptionsGroupLabel", grpDescriptions, caption, langMap);

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
        
        // For the last two fields, we need browse buttons
        if(i > GameObjectDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameObjectDescriptions::NumDescriptions;

            btnBrowse[currentButton].Create(*this, 0,
                                            WS_TABSTOP | BS_PUSHBUTTON);
            SetWindowTextFromLangMapString("BrowseButton", btnBrowse[currentButton], caption, langMap);
        }
        else {
            txtDescriptions[i].SetDlgCtrlID(ControlIDs::Name+i);
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

    btnBrowse[0].SetDlgCtrlID(ControlIDs::BrowseIcon);
    btnBrowse[1].SetDlgCtrlID(ControlIDs::BrowseSound);

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
/// moveControls - Move/resize the controls to their desired positions
/// @param a constant refrence to the dialog's window metrics object to be used
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

    pageHeight = cPos.y + CS.YWINDOW_MARGIN + CS.YUNRELATED_MARGIN;

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::populateFields(const GameObject &gameObject, const GameMap& gameMap) {

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {

        CString caption;

        SetWindowTextFromStr(gameObject.getDescription(i),
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
   
    if(txtDescriptions[4].GetTextLength() != 0) {
        if(!iconDescValidator.validate()) {
            return &iconDescValidator;
        }
    }

    if(txtDescriptions[5].GetTextLength() != 0) {
        if(!soundDescValidator.validate()) {
            return &soundDescValidator;
        }   
    }

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

    parentWindow->madeChange();
    return TRUE;

}
