#include "editor_mainwindow_views.h"
#include "../util/languagemapper.h"

//=============================================================================
// Public / Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Creates child controls. 
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int GameEntitiesView::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    SetStyle(GetStyle() | WS_CLIPCHILDREN);

    LanguageMapper& langMap = LanguageMapper::getInstance();

    CString caption = AtoW(langMap.get(LanguageConstants::ObjectGroupboxCaption).c_str(), CP_UTF8);

    objectsGroup.Create(*this, 0,WS_CLIPSIBLINGS);
    objectsGroup.SetWindowText(caption);

    caption = AtoW(langMap.get(LanguageConstants::CharacterGroupboxCaption).c_str(), CP_UTF8);

    charactersGroup.Create(*this, 0, WS_CLIPSIBLINGS);
    charactersGroup.SetWindowText(caption);

    objectsListBox.Create(*this, 0, WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);
    charactersListBox.Create(*this, 0, WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);

    for(int i = 0; i < 4; ++i) {

        caption = AtoW(langMap.get(LanguageConstants::ObjectAddButton + i).c_str(), CP_UTF8);
        alterObjectButton[i].Create(*this, 0, BS_PUSHBUTTON);
        alterObjectButton[i].SetWindowText(caption);

        caption = AtoW(langMap.get(LanguageConstants::CharacterAddButton + i).c_str(), CP_UTF8);
        alterCharacterButton[i].Create(*this, 0, BS_PUSHBUTTON);
        alterCharacterButton[i].SetWindowText(caption);
    }

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

void GameEntitiesView::updateLists(const std::vector<GameObject>& gameObjects) {
    objectsListBox.ClearStrings();

    for(std::vector<GameObject>::const_iterator it = gameObjects.begin();
        it != gameObjects.end(); ++it) {
            CString objectName = AtoW(it->getName().c_str(), CP_UTF8);
            objectsListBox.AddString(objectName);
    }
}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the view.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT GameEntitiesView::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    WINDOWPOS* wp;

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

    //objectsGroup.DeferWindowPos(hDWP, 0, objectGroupRect, SWP_NOZORDER);
    //charactersGroup.DeferWindowPos(hDWP, 0, characterGroupRect, SWP_NOZORDER);

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
