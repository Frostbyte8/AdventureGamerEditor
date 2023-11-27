#include "editor_mainwindow_views.h"
#include "shared_functions.h"
#include "../util/languagemapper.h"

//=============================================================================
// Control IDs
//=============================================================================

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
// GameEntitiesPanel
//
//=============================================================================

//=============================================================================
// Constructors / Destructor
//=============================================================================

// TODO: There is no reason for the Panel to be aware of the Main Window.

GameEntitiesPanel::GameEntitiesPanel(GameWorldController* gwc,
WindowMetrics* inWindowMetrics) : mainWindowController(gwc),
windowMetrics(inWindowMetrics) {
}

GameEntitiesPanel::~GameEntitiesPanel() {
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Creates child controls. 
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int GameEntitiesPanel::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();

    CString caption = AtoW(langMap.get("ObjectsGroup").c_str(), CP_UTF8);

    objectsGroup.Create(*this, 0, BS_GROUPBOX);
    objectsGroup.SetWindowText(caption);

    caption = AtoW(langMap.get("CharactersGroup").c_str(), CP_UTF8);

    charactersGroup.Create(*this, 0, WS_CLIPSIBLINGS | BS_GROUPBOX);
    charactersGroup.SetWindowText(caption);

    objectsListBox.Create(*this, 0, WS_CLIPSIBLINGS | WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);
    charactersListBox.Create(*this, 0, WS_CLIPSIBLINGS | WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);

    for (int i = 0; i < 4; ++i) {

        alterObjectButton[i].Create(*this, 0, WS_CLIPSIBLINGS | BS_PUSHBUTTON);
        alterObjectButton[i].SetWindowText(caption);
        alterObjectButton[i].SetDlgCtrlID(ControlIDs::AddObjectButton + i);

        alterCharacterButton[i].Create(*this, 0, WS_CLIPSIBLINGS | BS_PUSHBUTTON);
        alterCharacterButton[i].SetWindowText(caption);
        alterCharacterButton[i].SetDlgCtrlID(ControlIDs::AddCharacterButton + i);

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
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL GameEntitiesPanel::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        switch(ctrlID) {
            case ControlIDs::AddObjectButton:
                mainWindowController->tryAlterObject(AlterType::Add, 0);
                break;

            case ControlIDs::EditObjectButton:
            case ControlIDs::PlaceObjectButton:
            case ControlIDs::DeleteObjectButton:
                mainWindowController->tryAlterObject(ctrlID - ControlIDs::AddObjectButton, 
                                                     objectsListBox.GetCurSel());
                break;

            case ControlIDs::AddCharacterButton:
                mainWindowController->tryAlterCharacter(AlterType::Add, 0);
                break;

            case ControlIDs::EditCharacterButton:
            case ControlIDs::PlaceCharacterButton:
            case ControlIDs::DeleteCharacterButton:
                mainWindowController->tryAlterCharacter(ctrlID - ControlIDs::AddCharacterButton,
                                                        charactersListBox.GetCurSel());
                break;
        }

    }

    // TODO: When do we return TRUE?
    return FALSE;
}

///----------------------------------------------------------------------------
/// PreCreate - Sets settings for the child window before it is created. See 
/// the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void GameEntitiesPanel::PreCreate(CREATESTRUCT& cs) {
    cs.dwExStyle |= WS_EX_COMPOSITED;
}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the view.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT GameEntitiesPanel::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_SIZE:
        return OnSize(wParam, lParam);
        break;

    }

    return WndProcDefault(msg, wParam, lParam);
}

//=============================================================================
// Public / Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// updateCharacterList - Updates the character listbox.
/// @param a pointer to a vector with the list of characters in the game world. 
///----------------------------------------------------------------------------

void GameEntitiesPanel::updateCharacterList(const std::vector<GameCharacter>& characterList) {

    charactersListBox.ClearStrings();

    for(std::vector<GameCharacter>::const_iterator it = characterList.begin();
        it != characterList.end(); ++it) {

        CString charName = AtoW(std::to_string(it->getID()).c_str(), CP_UTF8);
        charName += ". ";
        charName += AtoW(it->getName().c_str(), CP_UTF8);
        charactersListBox.AddString(charName);

    }

}

///----------------------------------------------------------------------------
/// updateObjectList - Updates the object listbox.
/// @param a pointer to a vector with the list of objects in the game world. 
///----------------------------------------------------------------------------

void GameEntitiesPanel::updateObjectList(const std::vector<GameObject>& objectList) {

    objectsListBox.ClearStrings();

    for(std::vector<GameObject>::const_iterator it = objectList.begin();
        it != objectList.end(); ++it) {

        CString objectName = AtoW(std::to_string(it->getID()).c_str(), CP_UTF8);
        objectName += ". ";
        objectName += AtoW(it->getName().c_str(), CP_UTF8);
        objectsListBox.AddString(objectName);

    }

}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnSize - Processes the WM_SIZE message by moving/sizing the controls.
/// Refer to the Win32 on WM_SIZE for more information.
///----------------------------------------------------------------------------

int GameEntitiesPanel::OnSize(const WPARAM& wParam, const LPARAM& lParam) {

    const WindowMetrics::ControlDimensions cd = windowMetrics->GetControlDimensions();
    const WindowMetrics::ControlSpacing cs = windowMetrics->GetControlSpacing();

    const WORD xPos           = cs.XRELATED_MARGIN;
    const WORD yPos           = cs.YRELATED_MARGIN; 
    const WORD newWidth       = LOWORD(lParam) - (cs.XRELATED_MARGIN * 2);
    const WORD newHeight      = HIWORD(lParam) - (cs.YRELATED_MARGIN * 3);

    const CRect objectGroupRect(CPoint(xPos, yPos), CSize(newWidth, static_cast<int>(newHeight * 0.5)));

    const CRect characterGroupRect(CPoint(xPos, objectGroupRect.bottom + cs.YRELATED_MARGIN),
                                   CSize(newWidth, newHeight - objectGroupRect.Size().cy));
    
    HDWP hDWP = BeginDeferWindowPos(12);

    objectsGroup.DeferWindowPos(hDWP, HWND_TOP, objectGroupRect, SWP_NOREDRAW);
    charactersGroup.DeferWindowPos(hDWP, HWND_TOP, characterGroupRect, SWP_NOREDRAW);

    sizeGroupBox(hDWP, false, objectGroupRect, cs, cd);
    sizeGroupBox(hDWP, true, characterGroupRect, cs, cd);

    EndDeferWindowPos(hDWP);    

    RedrawWindow(RDW_UPDATENOW | RDW_ERASE | RDW_FRAME | RDW_INVALIDATE);
    UpdateWindow();

    objectsGroup.UpdateWindow();
    objectsListBox.UpdateWindow();
    charactersGroup.UpdateWindow();
    charactersListBox.UpdateWindow();

    for (int i = 0; i < 4; ++i) {
        alterObjectButton[i].UpdateWindow();
        alterCharacterButton[i].UpdateWindow();
    }

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

void GameEntitiesPanel::sizeGroupBox(HDWP& hDWP, const bool doCharacters, const CRect& dimensions,
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
    listBox->DeferWindowPos(hDWP, HWND_TOP, listBoxRect, SWP_NOREDRAW);
    //listBox->MoveWindow(listBoxRect, TRUE);

    const int width = listBoxRect.Size().cx;
    const int buttonWidth = static_cast<int>(width * 0.5);  

    // Then move the buttons
    CRect leftButtonRect(xPos, bottom, xPos + buttonWidth, bottom + cd.YBUTTON);

    const CPoint bottomRight = leftButtonRect.BottomRight();

    CRect rightButtonRect(bottomRight.x, bottom, 
                          bottomRight.x + (width - buttonWidth), bottom + cd.YBUTTON);
    
    buttons[0].DeferWindowPos(hDWP, HWND_TOP, leftButtonRect, SWP_NOREDRAW);
    buttons[1].DeferWindowPos(hDWP, HWND_TOP, rightButtonRect, SWP_NOREDRAW);

    leftButtonRect.MoveToY(bottomRight.y);
    rightButtonRect.MoveToY(bottomRight.y);

    buttons[2].DeferWindowPos(hDWP, HWND_TOP, leftButtonRect, SWP_NOREDRAW);
    buttons[3].DeferWindowPos(hDWP, HWND_TOP, rightButtonRect, SWP_NOREDRAW);

}

//=============================================================================
//
// EntitiesHereView
//
//=============================================================================

//=============================================================================
// Constructors / Destructor
//=============================================================================

EntitiesHerePanel::EntitiesHerePanel(WindowMetrics* inWindowMetrics) : 
windowMetrics(inWindowMetrics) {
}

EntitiesHerePanel::~EntitiesHerePanel() {
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Creates child controls. 
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EntitiesHerePanel::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();

    objectsHereGroup.Create(*this, 0, WS_CLIPSIBLINGS | BS_GROUPBOX);
    objectsHereGroup.SetWindowText(L"Objects Here");

    charactersHereGroup.Create(*this, 0, WS_CLIPSIBLINGS | BS_GROUPBOX);
    charactersHereGroup.SetWindowText(L"Chars here");

    objectsHereListBox.Create(*this, 0, WS_CLIPSIBLINGS | WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);
    charactersHereListBox.Create(*this, 0, WS_CLIPSIBLINGS | WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT);

    return retVal;
}

///----------------------------------------------------------------------------
/// PreCreate - Sets settings for the child window before it is created. See 
/// the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EntitiesHerePanel::PreCreate(CREATESTRUCT& cs) {
    cs.dwExStyle |= WS_EX_COMPOSITED;
}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the view.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT EntitiesHerePanel::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    if (msg == WM_SIZE) {
        return OnSize(wParam, lParam);
    }

    return WndProcDefault(msg, wParam, lParam);
}

//=============================================================================
// Public / Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// clearLists - Clears both lists.
///----------------------------------------------------------------------------

void EntitiesHerePanel::clearLists() {
    objectsHereListBox.ClearStrings();
    charactersHereListBox.ClearStrings();
}

///----------------------------------------------------------------------------
/// updateCharacterList - Updates the character's here listbox.
/// @param a pointer to a vector with the list of characters in the game world. 
///----------------------------------------------------------------------------

void EntitiesHerePanel::updateCharacterList(const std::vector<GameCharacter>& characterList) {

    charactersHereListBox.ClearStrings();

    if (!characterList.empty()) {
        const size_t charSize = characterList.size();

        for (size_t i = 0; i < charSize; ++i) {

            charactersHereListBox.AddString(AtoW(characterList[i].getName().c_str(), CP_UTF8));
        }
    }

}

///----------------------------------------------------------------------------
/// updateObjectList - Updates the object's here listbox.
/// @param a pointer to a vector with the list of objects in the game world. 
///----------------------------------------------------------------------------

void EntitiesHerePanel::updateObjectList(const std::vector<GameObject>& objectList) {

    objectsHereListBox.ClearStrings();

    if (!objectList.empty()) {
        const size_t objSize = objectList.size();

        for (size_t i = 0; i < objSize; ++i) {

            objectsHereListBox.AddString(AtoW(objectList[i].getName().c_str(), CP_UTF8));

        }
    }

}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnSize - Processes the WM_SIZE message.
///----------------------------------------------------------------------------

int EntitiesHerePanel::OnSize(const WPARAM& wParam, const LPARAM& lParam) {

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

    HDWP hDWP = BeginDeferWindowPos(4);

    objectsHereGroup.DeferWindowPos(hDWP, HWND_TOP, objectGroupRect, SWP_NOREDRAW);
    charactersHereGroup.DeferWindowPos(hDWP, HWND_TOP, characterGroupRect, SWP_NOREDRAW);
    objectsHereListBox.DeferWindowPos(hDWP, HWND_TOP, objectListRect, SWP_NOREDRAW);
    charactersHereListBox.DeferWindowPos(hDWP, HWND_TOP, characterListRect, SWP_NOREDRAW);

    EndDeferWindowPos(hDWP);

    RedrawWindow(RDW_UPDATENOW | RDW_ERASE | RDW_FRAME | RDW_INVALIDATE);
    UpdateWindow();

    objectsHereGroup.UpdateWindow();
    charactersHereGroup.UpdateWindow();
    objectsHereListBox.UpdateWindow();
    charactersHereListBox.UpdateWindow();

    return 0;
}

//=============================================================================
//
// RoadPalettePanel
//
//=============================================================================

//=============================================================================
// Constructors / Destructor
//=============================================================================

RoadPalettePanel::RoadPalettePanel(MainWindowInterface* inMainWindow, GameWorldController* gwc) :
gameWorldController(gwc), backBufferDC(0), tilesetDC(0), mainWindow(inMainWindow) {
}

RoadPalettePanel::~RoadPalettePanel() {
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setTileset - Copies the tileset bitmap into it's own DC for drawing, then
/// makes sure the backbuffer gets updated.
///----------------------------------------------------------------------------

void RoadPalettePanel::setTileset(CBitmap& bmp) {

    BITMAP bm;
    bmp.GetObject(sizeof(BITMAP), &bm);
    CMemDC memDC(NULL);
    HBITMAP oldBMP = memDC.SelectObject(bmp);
    tilesetDC.CreateCompatibleBitmap(memDC, bm.bmWidth, bm.bmHeight);
    tilesetDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY);

    tileWidth = bm.bmWidth / EditorConstants::TilesPerCol;
    tileHeight = bm.bmHeight / EditorConstants::TilesPerRow;

    memDC.SelectObject(oldBMP);

    updateScrollSize();
    updateBackBuffer();
    InvalidateRect();

}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCreate - Create the out of Bounds Brush color so the background is black.
/// where there are no visible tiles.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int RoadPalettePanel::OnCreate(CREATESTRUCT& cs) {

    CBrush outOfBoundsColor(RGB(0, 0, 0));
    SetScrollBkgnd(outOfBoundsColor);

    return CScrollView::OnCreate(cs);
}

///----------------------------------------------------------------------------
/// OnDraw - Presents the back buffer as the currently visible frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void RoadPalettePanel::OnDraw(CDC& dc) {
    if (backBufferBMP.GetHandle()) {
        dc.SelectObject(backBufferBMP);
    }
}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the view.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT RoadPalettePanel::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    if (msg == WM_LBUTTONDOWN) {
        return onLButtonDown(LOWORD(lParam), HIWORD(lParam));
    }

    return WndProcDefault(msg, wParam, lParam);
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// onDrawingTileChanged - Received when the tile being drawn with has changed.
///----------------------------------------------------------------------------

void RoadPalettePanel::onDrawingTileChanged() {
    updateBackBuffer();
    InvalidateRect(0);
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// onLButtonDown - Processes the WM_LBUTTONDOWN window message
/// @param x coordinate on the road palette where the left mouse button was
/// pressed.
/// @param y coordinate on the road palette where the left mouse button was
/// pressed.
/// @return always 0.
///----------------------------------------------------------------------------

LRESULT RoadPalettePanel::onLButtonDown(const WORD &x, const WORD &y) {

    SCROLLINFO si ={ 0 };
    si.cbSize = sizeof(SCROLLBARINFO);
    si.fMask = SIF_POS;
    GetScrollInfo(SB_VERT, si);

    const int newTileSelected =  (y + si.nPos) / tileHeight;
    
    gameWorldController->trySetDrawingTile(newTileSelected);

    return 0;
}

///----------------------------------------------------------------------------
/// updateBackBuffer - Draws the next frame of the back buffer
///----------------------------------------------------------------------------

void RoadPalettePanel::updateBackBuffer() {

    const int totalHeight = tileHeight * (EditorConstants::TilesPerRow * 2);

    // TODO: Verify that the previous BMP is destroyed as per what the docs
    // state?

    CClientDC dc(*this);
    backBufferBMP = CreateCompatibleBitmap(dc, tileWidth, totalHeight);

    CBitmap oldBMP;
    oldBMP = backBufferDC.SelectObject(backBufferBMP);


    for (int i = 0; i < EditorConstants::TilesPerRow; ++i) {

        // For each iteration, we draw two tiles: One for the normal tile,
        // and one for it's dirt road version

        const int yDest = i * tileHeight;
        
        const int yDirtDest = (i * tileHeight) + 
                              (EditorConstants::TilesPerRow * tileHeight);

        const int xSrc = i * tileWidth;

        const int yDirtSrc = EditorConstants::DirtroadStartRow * tileHeight;

        backBufferDC.BitBlt(0, yDest, tileWidth, tileHeight, 
                            tilesetDC, xSrc, 0, SRCCOPY);

        backBufferDC.BitBlt(0, yDirtDest, tileWidth, tileHeight,
                            tilesetDC, xSrc, yDirtSrc, SRCCOPY);

    }

    // Finally, we will draw a selection box around the tile that the user
    // is currently drawing with.

    const int selectedTile = gameWorldController->getDrawingTileIndex();

    DrawTileSelectionBox(backBufferDC, 0, selectedTile * tileHeight,
                         tileWidth, tileHeight, 2);

    backBufferDC.SelectObject(oldBMP);

}

///----------------------------------------------------------------------------
/// updateScrollSize - Sets the scroll info for the view based on the current
/// tileset
///----------------------------------------------------------------------------

void RoadPalettePanel::updateScrollSize() {

    // There are 16 tile types, plus an additional 16 for dirt roads.
    const int totalHeight = tileHeight * (EditorConstants::TilesPerRow * 2);
    CSize scrollSize(tileWidth, totalHeight);
    SetScrollSizes(scrollSize);

}