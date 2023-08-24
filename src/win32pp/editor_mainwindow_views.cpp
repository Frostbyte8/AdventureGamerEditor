#include "editor_mainwindow_views.h"
#include "shared_functions.h"
#include "../util/languagemapper.h"

namespace ControlIDs {

    const WORD AddObjectButton          = 201;
    const WORD EditObjectButton         = 202;
    const WORD PlaceObjectButton        = 203;
    const WORD DeleteObjectButton       = 204;

    const WORD AddCharacterButton       = 205;
    const WORD EditCharacterButton      = 206;
    const WORD PlaceCharacterButton     = 207;
    const WORD DeleteCharacterButton    = 208;

}

//=============================================================================
//
// GameEntitiesView
//
//=============================================================================

//=============================================================================
// Public / Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL GameEntitiesView::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if(ctrlID == ControlIDs::AddObjectButton) {
            mainWindow->onAlterObject(AlterType::Add, 0);
        }
        else if(ctrlID == ControlIDs::AddCharacterButton) {
            mainWindow->onAlterCharacter(AlterType::Add, 0);
        }
        else if(ctrlID == ControlIDs::EditObjectButton) {
            mainWindow->onAlterObject(AlterType::Edit, objectsListBox.GetCurSel());
        }
        else if(ctrlID == ControlIDs::EditCharacterButton) {
            mainWindow->onAlterCharacter(AlterType::Edit, charactersListBox.GetCurSel());
        }

    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Creates child controls. 
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int GameEntitiesView::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();

    CString caption = AtoW(langMap.get("ObjectsGroup").c_str(), CP_UTF8);

    objectsGroup.Create(*this, 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX);
    objectsGroup.SetWindowText(caption);

    caption = AtoW(langMap.get("CharactersGroup").c_str(), CP_UTF8);

    charactersGroup.Create(*this, 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX);
    charactersGroup.SetWindowText(caption);

    objectsListBox.Create(*this, 0, WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);
    charactersListBox.Create(*this, 0, WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);

    for(int i = 0; i < 4; ++i) {

        alterObjectButton[i].Create(*this, 0, BS_PUSHBUTTON);
        alterObjectButton[i].SetWindowText(caption);
        alterObjectButton[i].SetDlgCtrlID(ControlIDs::AddObjectButton+i);

        alterCharacterButton[i].Create(*this, 0, BS_PUSHBUTTON);
        alterCharacterButton[i].SetWindowText(caption);
        alterCharacterButton[i].SetDlgCtrlID(ControlIDs::AddCharacterButton+i);
        
    }

    alterObjectButton[0].SetWindowText(LM_toUTF8("AddButton", langMap));
    alterObjectButton[1].SetWindowText(LM_toUTF8("EditButton", langMap));
    alterObjectButton[2].SetWindowText(LM_toUTF8("PlaceButton", langMap));
    alterObjectButton[3].SetWindowText(LM_toUTF8("DeleteButton", langMap));
    alterCharacterButton[0].SetWindowText(LM_toUTF8("AddButton", langMap));
    alterCharacterButton[1].SetWindowText(LM_toUTF8("EditButton", langMap));
    alterCharacterButton[2].SetWindowText(LM_toUTF8("PlaceButton", langMap));
    alterCharacterButton[3].SetWindowText(LM_toUTF8("DeleteButton", langMap));

    return retVal;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Called before the class is registered. Using it to set
/// the background color of the view.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void GameEntitiesView::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"GameEntitiesView";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.style = CS_HREDRAW;
}

///----------------------------------------------------------------------------
/// updateLists - Test function for populating the list boxes
///----------------------------------------------------------------------------

void GameEntitiesView::updateLists(const std::vector<GameObject>& gameObjects, const std::vector<GameCharacter>& gameCharacters) {

    objectsListBox.ClearStrings();

    for(std::vector<GameObject>::const_iterator it = gameObjects.begin();
        it != gameObjects.end(); ++it) {

            CString objectName = AtoW(std::to_string(it->getID()).c_str(), CP_UTF8);
            objectName += ". ";
            objectName += AtoW(it->getName().c_str(), CP_UTF8);

            //CString objectName = AtoW(it->getName().c_str(), CP_UTF8);
            objectsListBox.AddString(objectName);
    }


    charactersListBox.ClearStrings();

    for(std::vector<GameCharacter>::const_iterator it = gameCharacters.begin();
        it != gameCharacters.end(); ++it) {
            CString charName = AtoW(std::to_string(it->getID()).c_str(), CP_UTF8);
            charName += ". ";
            charName += AtoW(it->getName().c_str(), CP_UTF8);
            charactersListBox.AddString(charName);
    }
}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the view.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT GameEntitiesView::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    //WINDOWPOS* wp;

    switch(msg) {
        case WM_SIZE: return OnSize(wParam, lParam);
           /*
        case WM_WINDOWPOSCHANGING:
            wp = (WINDOWPOS*)(lParam);
            wp->flags |= SWP_NOCOPYBITS;
            break;
            */
    }

    return WndProcDefault(msg, wParam, lParam);
}
//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnSize - Processes the WM_SIZE message by moving/sizing the controls.
/// Refer to the Win32 on WM_SIZE for more information.
///----------------------------------------------------------------------------

int GameEntitiesView::OnSize(const WPARAM& wParam, const LPARAM& lParam) {

    const WindowMetrics::ControlDimensions cd = windowMetrics->GetControlDimensions();
    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();

    const WORD xPos           = cs.XRELATED_MARGIN;
    const WORD yPos           = cs.YRELATED_MARGIN; 
    const WORD newWidth       = LOWORD(lParam) - (cs.XRELATED_MARGIN * 2);
    const WORD newHeight      = HIWORD(lParam) - (cs.YRELATED_MARGIN * 3);

    const CRect objectGroupRect(CPoint(xPos, yPos), CSize(newWidth, static_cast<int>(newHeight * 0.5)));

    const CRect characterGroupRect(CPoint(xPos, objectGroupRect.bottom + cs.YRELATED_MARGIN),
                                   CSize(newWidth, newHeight - objectGroupRect.Size().cy));

    HDWP hDWP = BeginDeferWindowPos(10);


    //objectsGroup.MoveWindow(objectGroupRect, TRUE);
    //charactersGroup.MoveWindow(characterGroupRect, FALSE);

    objectsGroup.DeferWindowPos(hDWP, 0, objectGroupRect, SWP_NOZORDER);
    charactersGroup.DeferWindowPos(hDWP, 0, characterGroupRect, SWP_NOZORDER);

    sizeGroupBox(hDWP, false, objectGroupRect, cs, cd);
    sizeGroupBox(hDWP, true, characterGroupRect, cs, cd);

    EndDeferWindowPos(hDWP);

    return 0;
}

///----------------------------------------------------------------------------
/// sizeGroupBox - Sizes the group box controls based on the dimensions given.
/// @param doCharacters - If true, move the controls associated with the
/// characters groupbox, if false, move the controls associated with the
/// objects groupbox,
/// @param A CRect containing the dimensions of the groupbox.
/// @param Control Spacing from Window Metrics
/// @param Control Dimensions from Window Metrics
/// TODO: Function name is misleading. Fix that.
///----------------------------------------------------------------------------

void GameEntitiesView::sizeGroupBox(HDWP& hDWP, const bool doCharacters, const CRect& dimensions,
                                    const WindowMetrics::ControlSpacing& cs, 
                                    const WindowMetrics::ControlDimensions& cd) {

    CListBox* listBox;
    CButton*  buttons;

    if(doCharacters) {
        listBox = &charactersListBox;
        buttons = alterCharacterButton;
    }
    else {
        listBox = &objectsListBox;
        buttons = alterObjectButton;
    }

    const int totalButtonHeight = cd.YBUTTON * 2;
    const int xPos      = dimensions.left + cs.XGROUPBOX_MARGIN;
    const int yPos      = dimensions.top + cs.YFIRST_GROUPBOX_MARGIN;
    const int right     = dimensions.right - cs.XGROUPBOX_MARGIN;
    const int bottom    = dimensions.bottom - (totalButtonHeight + cs.YWINDOW_MARGIN);
    
    // Start by moving the list box
    const CRect listBoxRect(xPos, yPos, right, bottom);
    listBox->DeferWindowPos(hDWP, NULL, listBoxRect, SWP_NOZORDER | SWP_NOCOPYBITS);
    //listBox->MoveWindow(listBoxRect, TRUE);

    const int width = listBoxRect.Size().cx;
    const int buttonWidth = static_cast<int>(width * 0.5);  

    // Then move the buttons
    CRect leftButtonRect(xPos, bottom, xPos + buttonWidth, bottom + cd.YBUTTON);

    const CPoint bottomRight = leftButtonRect.BottomRight();

    CRect rightButtonRect(bottomRight.x, bottom, 
                          bottomRight.x + (width - buttonWidth), bottom + cd.YBUTTON);
    
    buttons[0].DeferWindowPos(hDWP, NULL, leftButtonRect, SWP_NOZORDER | SWP_NOCOPYBITS); //MoveWindow(leftButtonRect, TRUE);
    buttons[1].DeferWindowPos(hDWP, NULL, rightButtonRect, SWP_NOZORDER | SWP_NOCOPYBITS);

    leftButtonRect.MoveToY(bottomRight.y);
    rightButtonRect.MoveToY(bottomRight.y);

    buttons[2].DeferWindowPos(hDWP, NULL, leftButtonRect, SWP_NOZORDER | SWP_NOCOPYBITS);
    buttons[3].DeferWindowPos(hDWP, NULL, rightButtonRect, SWP_NOZORDER | SWP_NOCOPYBITS);

}

//=============================================================================
//
// EntitiesHereView
//
//=============================================================================

int EntitiesHereView::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();

    objectsHereGroup.Create(*this, 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX);
    objectsHereGroup.SetWindowText(L"Objects Here");

    charactersHereGroup.Create(*this, 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX);
    charactersHereGroup.SetWindowText(L"Chars here");

    objectsHereListBox.Create(*this, 0, WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);
    charactersHereListBox.Create(*this, 0, WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);

    return retVal;
}

int EntitiesHereView::OnSize(const WPARAM& wParam, const LPARAM& lParam) {

    const WindowMetrics::ControlDimensions cd = windowMetrics->GetControlDimensions();
    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();

    const WORD xPos = cs.XRELATED_MARGIN;
    const WORD yPos = cs.YRELATED_MARGIN;
    const WORD newWidth = LOWORD(lParam) - (cs.XRELATED_MARGIN * 2);
    const WORD newHeight = HIWORD(lParam) - (cs.YRELATED_MARGIN * 2);

    const CRect objectGroupRect(CPoint(xPos, yPos),
                                CSize(static_cast<int>(newWidth * 0.5), newHeight));

    const CRect characterGroupRect(CPoint(objectGroupRect.right + cs.XRELATED_MARGIN, yPos),
                                   CSize(newWidth - objectGroupRect.Size().cx, newHeight));

    const CRect objectListRect(objectGroupRect.left + cs.XGROUPBOX_MARGIN,
                               objectGroupRect.top + cs.YFIRST_GROUPBOX_MARGIN,
                               objectGroupRect.right - cs.XGROUPBOX_MARGIN,
                               objectGroupRect.bottom - cs.YWINDOW_MARGIN);
                               
    const CRect characterListRect(characterGroupRect.left + cs.XGROUPBOX_MARGIN,
                                  characterGroupRect.top + cs.YFIRST_GROUPBOX_MARGIN,
                                  characterGroupRect.right - cs.XGROUPBOX_MARGIN,
                                  characterGroupRect.bottom - cs.YWINDOW_MARGIN);

    objectsHereGroup.MoveWindow(objectGroupRect);
    charactersHereGroup.MoveWindow(characterGroupRect);
    objectsHereListBox.MoveWindow(objectListRect);
    charactersHereListBox.MoveWindow(characterListRect);
    

    return 0;
}

LRESULT EntitiesHereView::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    if (msg == WM_SIZE) {
        return OnSize(wParam, lParam);
    }

    return WndProcDefault(msg, wParam, lParam);
}

void EntitiesHereView::updateLists(const std::vector<GameObject>& objectVec, const std::vector<GameCharacter>& charVec) {
    
    objectsHereListBox.ClearStrings();
    charactersHereListBox.ClearStrings();
    
    if(!objectVec.empty()) {
        const size_t objSize = objectVec.size();

        for(size_t i = 0; i < objSize; ++i) {
            
            objectsHereListBox.AddString(AtoW(objectVec[i].getName().c_str(), CP_UTF8));

        }
    }

    if(!charVec.empty()) {
        const size_t charSize = charVec.size();

        for (size_t i = 0; i < charSize; ++i) {

            charactersHereListBox.AddString(AtoW(charVec[i].getName().c_str(), CP_UTF8));
        }
    }
}