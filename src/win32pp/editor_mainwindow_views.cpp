#include "editor_mainwindow_views.h"

int GameEntitiesView::OnCreate(CREATESTRUCT& css) {

    const int retVal = CWnd::OnCreate(css);

    objectsGroup.Create(*this, 0, BS_GROUPBOX);
    objectsGroup.SetWindowText(L"Objects");

    charactersGroup.Create(*this, 0, BS_GROUPBOX);
    charactersGroup.SetWindowText(L"Characters");

    objectsListBox.Create(*this, 0, WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);
    charactersListBox.Create(*this, 0, WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);

    for(int i = 0; i < 4; ++i) {
        alterObjectButton[i].Create(*this);
        alterCharacterButton[i].Create(*this, 0, BS_PUSHBUTTON);
    }

    return retVal;
}

int GameEntitiesView::OnSize(const WPARAM& wParam, const LPARAM& lParam) {

    const WindowMetrics::ControlDimensions cd = windowMetrics->GetControlDimensions();
    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();

    WORD xPos           = cs.XRELATED_MARGIN;
    WORD yPos           = cs.YRELATED_MARGIN; 
    WORD newWidth       = LOWORD(lParam);
    WORD newHeight      = HIWORD(lParam);

    newWidth    -= cs.XRELATED_MARGIN * 2;
    newHeight   -= (cs.YRELATED_MARGIN * 3);

    const CRect objectGroupRect(CPoint(xPos, yPos), CSize(newWidth, static_cast<int>(newHeight * 0.5)));

    const CRect characterGroupRect(CPoint(xPos, objectGroupRect.bottom + cs.YRELATED_MARGIN),
                                   CSize(newWidth, newHeight - objectGroupRect.Size().cy));

    objectsGroup.MoveWindow(objectGroupRect, TRUE);
    charactersGroup.MoveWindow(characterGroupRect, TRUE);

    sizeGroupBox(false, objectGroupRect, cs, cd);
    sizeGroupBox(true, characterGroupRect, cs, cd);

    return 0;
}

void GameEntitiesView::sizeGroupBox(const bool doCharacters, const CRect& dimensions,
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
    listBox->MoveWindow(listBoxRect, TRUE);

    const int width = listBoxRect.Size().cx;
    const int buttonWidth = static_cast<int>(width * 0.5);  

    // Then move the buttons
    CRect leftButtonRect(xPos, bottom, xPos + buttonWidth, bottom + cd.YBUTTON);

    const CPoint bottomRight = leftButtonRect.BottomRight();

    CRect rightButtonRect(bottomRight.x, bottom, 
                          bottomRight.x + (width - buttonWidth), bottom + cd.YBUTTON);
    
    buttons[0].MoveWindow(leftButtonRect, TRUE);
    buttons[1].MoveWindow(rightButtonRect, TRUE);

    leftButtonRect.MoveToY(bottomRight.y);
    rightButtonRect.MoveToY(bottomRight.y);

    buttons[2].MoveWindow(leftButtonRect, TRUE);
    buttons[3].MoveWindow(rightButtonRect, TRUE);

}

LRESULT GameEntitiesView::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    switch(msg) {
        case WM_SIZE: return OnSize(wParam, lParam);
    }

    return WndProcDefault(msg, wParam, lParam);
}
