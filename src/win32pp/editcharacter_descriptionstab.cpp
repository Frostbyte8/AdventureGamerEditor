#include "editcharacter_tabviews.h"
#include "../model/gamecharacter.h"
#include <wxx_commondlg.h>

#include "shared_functions.h"

namespace ControlIDs {
    const WORD NameText     = 101;
    const WORD OnSightText  = 102;
    const WORD OnFightText  = 103;
    const WORD OnDeathText  = 104;
    const WORD BrowseIcon   = 105;
    const WORD BrowseSound  = 106;
    
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterDescriptionsTab::OnCommand(WPARAM wParam, LPARAM lParam) {
    
    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD ctrlAction = HIWORD(wParam);

        if(ctrlID == ControlIDs::BrowseIcon || ctrlID == ControlIDs::BrowseSound) {
            if(ctrlAction == BN_CLICKED) {
                return onBrowseForMedia(ctrlID == ControlIDs::BrowseIcon ? true : false);
            }
        }
        else if(ctrlID >= ControlIDs::NameText && ctrlID <= ControlIDs::OnDeathText) {
            if(ctrlAction == EN_CHANGE) {
                parentWindow->madeChange();
            }
        }

    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the tab, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditCharacterDescriptionsTab::OnCreate(CREATESTRUCT& cs) {
    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    grpDescriptions.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText("CharDescriptionsGroup", grpDescriptions, caption, langMap);

    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        lblDescriptions[i].Create(*this, 0, SS_SIMPLE);
        txtDescriptions[i].Create(*this, 0, ES_AUTOHSCROLL | WS_TABSTOP);
        txtDescriptions[i].SetExStyle(WS_EX_CLIENTEDGE);

        if(i > 3) {
            btnBrowse[i-4].Create(*this, 0, BS_PUSHBUTTON);
            btnBrowse[i-4].SetDlgCtrlID(ControlIDs::BrowseIcon + (i-4));
            
            txtDescriptions[i].EnableWindow(FALSE);
        }
        else {
            txtDescriptions[i].SetDlgCtrlID(ControlIDs::NameText+i);
        }
    } 
    
    EOD_SetWindowText("CharNameLabel", lblDescriptions[0], caption, langMap);
    EOD_SetWindowText("CharOnSightLabel", lblDescriptions[1], caption, langMap);
    EOD_SetWindowText("CharOnFightLabel", lblDescriptions[2], caption, langMap);
    EOD_SetWindowText("CharOnDeathLabel", lblDescriptions[3], caption, langMap);
    EOD_SetWindowText("CharIconLabel", lblDescriptions[4], caption, langMap);
    EOD_SetWindowText("CharSoundLabel", lblDescriptions[5], caption, langMap);
    EOD_SetWindowText("BrowseButton", btnBrowse[0], caption, langMap);
    EOD_SetWindowText("BrowseButton", btnBrowse[1], caption, langMap);

    std::vector<std::string> imageExtensions;
    imageExtensions.push_back(".ICO");
    imageExtensions.push_back(".BMP");

    iconDescValidator = StringValidator(&txtDescriptions[4], NULL, &imageExtensions, 12, 5);
    soundDescValidator = StringValidator(&txtDescriptions[5], "", ".WAV", 12, 5);

    return retVal;
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

///----------------------------------------------------------------------------
/// calculateWidth - Calculates the width of the tab page.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::calculatePageWidth(const WindowMetrics& windowMetrics) {
    pageWidth = 0;

    // The Width will be the size of the widest control, in this case, it will be
    // one of the labels, so we will loop through that.

    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        pageWidth = std::max(windowMetrics.CalculateStringWidth(
                             lblDescriptions[i].GetWindowTextW().c_str()), pageWidth);
    }

    const WindowMetrics::ControlSpacing cs = windowMetrics.GetControlSpacing();
    pageWidth += (cs.XGROUPBOX_MARGIN * 2) + (cs.XWINDOW_MARGIN * 2);
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::insertData(GameCharacter::Builder& builder) {

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
/// moveControls -
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::moveControls(const WindowMetrics& windowMetrics) {

    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();
    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();

    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (CS.XGROUPBOX_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultEditSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XGROUPBOX_MARGIN + CS.XWINDOW_MARGIN, 
                CS.YFIRST_GROUPBOX_MARGIN + CS.YRELATED_MARGIN + CS.YWINDOW_MARGIN);

    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {

        lblDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtDescriptions[i].MoveWindow(cPos.x, cPos.y,
                                      defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

        if(i > GameCharacterDescriptions::NumDescriptions - 1) {

            const int currentButton = i - GameCharacterDescriptions::NumDescriptions;

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


    pageHeight = cPos.y + CS.YUNRELATED_MARGIN + CS.YWINDOW_MARGIN;
}

///----------------------------------------------------------------------------
/// populateFields -
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::populateFields(const GameCharacter& gameCharacter) {

    CString caption;

    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        EOD_SetWindowText(gameCharacter.getDescription(i), txtDescriptions[i], caption);
    }
}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterDescriptionsTab::validateFields() {
   
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

// TODO: Split this into a shared function

BOOL EditCharacterDescriptionsTab::onBrowseForMedia(const bool findIcon) {

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

        parentWindow->madeChange();
	}

    return TRUE;

}
