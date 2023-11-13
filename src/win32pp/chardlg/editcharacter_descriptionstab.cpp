#include <wxx_commondlg.h>
#include "../shared_functions.h"
#include "../../model/gamecharacter.h"
#include "editcharacter_tabviews.h"

using namespace EditCharacterDialogConstants::DescriptionsTab;

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
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::insertData(GameCharacter::Builder& builder) {

    CString wideDesc = txtDescriptions[0].GetWindowText().Left(GameCharacterConstants::MaxCharacterName);
    builder.description(WtoA(wideDesc).c_str(), 0);

    wideDesc = txtDescriptions[1].GetWindowText().Left(GameCharacterConstants::MaxDescriptionLength);
    builder.description(WtoA(wideDesc).c_str(), 1);

    wideDesc = txtDescriptions[2].GetWindowText().Left(GameCharacterConstants::MaxFightText);
    builder.description(WtoA(wideDesc).c_str(), 2);

    wideDesc = txtDescriptions[3].GetWindowText().Left(GameCharacterConstants::MaxDeathText);
    builder.description(WtoA(wideDesc).c_str(), 3);

    wideDesc = txtDescriptions[4].GetWindowText();
    builder.description(WtoA(wideDesc).c_str(), 4);

    wideDesc = txtDescriptions[5].GetWindowText();
    builder.description(WtoA(wideDesc).c_str(), 5);

}



///----------------------------------------------------------------------------
/// populateFields - Fill the tab page with the relevant data from the
/// Game Character being worked on.
/// @param a constant reference to the Game Character containing the data to be
/// used to fill out the fields.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::populateFields(const GameCharacter& gameCharacter) {

    CString caption;

    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        SetWindowTextFromStr(gameCharacter.getDescription(i), txtDescriptions[i], caption);
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterDescriptionsTab::validateFields() {
   
    // For this tab page, only the last two text boxes need to be validated.

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
/// @return always TRUE.
///----------------------------------------------------------------------------

// TODO: Split this into a shared function since the other dialog uses the
// exact same thing.

BOOL EditCharacterDescriptionsTab::onBrowseForMedia(const bool findIcon) {

	CFileDialog fileDialog(TRUE, NULL, NULL, OFN_NOLONGNAMES | OFN_FILEMUSTEXIST, NULL);

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if(findIcon) {

        CString dialogTitle = AtoW(langMap.get("CDBFindImageTitle").c_str(), CP_UTF8);
        CString filterCaption = AtoW(langMap.get("CBDFindImageFilterText").c_str(), CP_UTF8);
        filterCaption += L" (*.BMP;*.ICO)|*.BMP;*.ICO|";

        fileDialog.SetFilter(filterCaption);
        fileDialog.SetTitle(dialogTitle);

    }
    else {

        CString dialogTitle = AtoW(langMap.get("CBDFindSoundTitle").c_str(), CP_UTF8);
        CString filterCaption = AtoW(langMap.get("CBDFindSoundFilterText").c_str(), CP_UTF8);
        filterCaption += L" (*.WAV)|*.WAV|";

        fileDialog.SetFilter(filterCaption);
        fileDialog.SetTitle(dialogTitle);

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

            CString errText = AtoW(langMap.get("ErrPathConversionText").c_str(), CP_UTF8);
            CString errTitle = AtoW(langMap.get("ErrPathConversionTitle").c_str(), CP_UTF8);

            MessageBox(errText, errTitle, MB_OK | MB_ICONERROR);
            return TRUE;
        }

        fileName = fileName.Mid(lastSlash, fileName.GetLength() - lastSlash); 

        txtDescriptions[(findIcon ? 4 : 5)].SetWindowText(fileName);

        parentWindow->madeChange();
	}

    return TRUE;

}
