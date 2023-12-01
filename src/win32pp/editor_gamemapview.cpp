#include "editor_mainwindow_views.h"
#include "../editor_constants.h"
#include "shared_functions.h"

//=============================================================================
// Constructors / Destructor
//=============================================================================

GameMapPanel::GameMapPanel(Win32ppMainWindowInterface* inMainWindow, GameWorldController* gwc) :
backBufferDC(NULL), tilesetDC(NULL), mainWindow(inMainWindow), gameWorldController(gwc) {
    zoomFactor = 1;
    tileWidth = 0;
    tileHeight = 0;
    scaledTileWidth = 0;
    scaledTileHeight = 0;
}

GameMapPanel::~GameMapPanel() {
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setTileset - Copies the tileset bitmap into it's own DC for drawing, then
/// makes sure the backbuffer gets updated.
///----------------------------------------------------------------------------

void GameMapPanel::setTileset(CBitmap& bmp) {

    BITMAP bm;
    bmp.GetObject(sizeof(BITMAP), &bm);
    CMemDC memDC(NULL);
    HBITMAP oldBMP = memDC.SelectObject(bmp);
    tilesetDC.CreateCompatibleBitmap(memDC, bm.bmWidth, bm.bmHeight);

    tilesetDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(oldBMP);

    tileWidth = bm.bmWidth / EditorConstants::TilesPerRow;
    tileHeight = bm.bmHeight / EditorConstants::TilesPerCol;
    scaledTileWidth = tileWidth * zoomFactor;
    scaledTileHeight = tileHeight * zoomFactor;

    updateBackBuffer();
    updateScrollSize();
    InvalidateRect();

}

///----------------------------------------------------------------------------
/// setZoomFactor - Changes the how zoomed in the game map is and redraws
/// the map to reflect this.
/// @param new zoom factor. Minimum of 1, max of 4. Invalid values here result
/// in an assertion warning.
///----------------------------------------------------------------------------

void GameMapPanel::setZoomFactor(const int& newZoomFactor) {
    
    assert(newZoomFactor > 0 && newZoomFactor < 5);
    
    zoomFactor = newZoomFactor;
    scaledTileWidth = tileWidth * zoomFactor;
    scaledTileHeight = tileHeight * zoomFactor;

    updateBackBuffer();
    updateScrollSize();
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

int GameMapPanel::OnCreate(CREATESTRUCT& cs) {
    
    CBrush outOfBoundsColor(RGB(0, 0, 0));
    SetScrollBkgnd(outOfBoundsColor);
    
    return CScrollView::OnCreate(cs);

}

///----------------------------------------------------------------------------
/// OnDraw - Draw the Game map to the back buffer, then present the backbuffer
/// to the view.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void GameMapPanel::OnDraw(CDC& dc) {

    if (backBufferBMP.GetHandle()) {
        dc.SelectObject(backBufferBMP);
    }

}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the view.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT GameMapPanel::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
        
        case WM_LBUTTONDOWN:    
            return onLButtonDown(LOWORD(lParam), HIWORD(lParam));

        case WM_LBUTTONDBLCLK:
            return onLButtonDBLClick(LOWORD(lParam), HIWORD(lParam));

        case WM_RBUTTONDOWN:
            return onRButtonDown(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_KEYDOWN:
            return onKeyDown(wParam, static_cast<WORD>(lParam));
            break;
    }

    return WndProcDefault(msg, wParam, lParam);
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// onMapSizeChanged - Update the back buffer and scrollbar sizes when the
/// size of the map changes.
///----------------------------------------------------------------------------

void GameMapPanel::onMapSizeChanged() {
    updateBackBuffer();
    updateScrollSize();
    InvalidateRect();
}

///----------------------------------------------------------------------------
/// onNewTileSelected - The tile selected has changed, so update the backbuffer
/// to reflect that. TODO: update and redraw the affected tiles.
///----------------------------------------------------------------------------

void GameMapPanel::onNewTileSelected() {
    updateBackBuffer();
    InvalidateRect();
}

///----------------------------------------------------------------------------
/// onTileUpdated - A tile was updated somewhere on the map, so update the
/// back buffer. TODO: update and redraw the affected tiles.
///----------------------------------------------------------------------------

void GameMapPanel::onTileUpdated() {
    updateBackBuffer();
    InvalidateRect();
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// onKeyDown - Processes the WM_KWYDOWN message. 
/// @param virtual key of the key pressed
/// @param various data pertaining to the key press
/// @return Always 0
///----------------------------------------------------------------------------

LRESULT GameMapPanel::onKeyDown(const WORD& vKey, const WORD& keyData) {

    switch(vKey) {

        case 'T':
            gameWorldController->tryToggleTileDarkness();
            break;

        case 'L':
            gameWorldController->tryToggleSwitchState();
            break;

        case 'J':
            gameWorldController->tryCreateJumpConnection();
            break;

        case 'S':
            gameWorldController->tryCreateSwitchConnection();
            break;

        case VK_SPACE:
            gameWorldController->tryDrawOnSelectedTile();
            break;

        case VK_RETURN:
            gameWorldController->tryEditTileDescription();
            break;

        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
            gameWorldController->trySelectNewTileDirection(vKey - VK_LEFT);
            break;
    }

    return 0;
}

///----------------------------------------------------------------------------
/// onLButtonDown - Processes the WM_LBUTTONDOWN message. It will request
/// that the controller change the selected tile to the one under the mouse.
/// If that change happens, it will update the map.
/// @param xPos of the mouse
/// @param yPos of the mouse
/// @return Always 0
///----------------------------------------------------------------------------

LRESULT GameMapPanel::onLButtonDown(const WORD& xPos, const WORD& yPos) {

    if (!gameWorldController->isWorldLoaded()) {
        return 0;
    }

    CPoint viewOffset = GetScrollPosition();

    int row = static_cast<int>((yPos + viewOffset.y) / scaledTileHeight);
    int col = static_cast<int>((xPos + viewOffset.x) / scaledTileWidth);

    // No need to verify the row and col, the function will do that for us.

    gameWorldController->trySelectNewTile(row, col);

    SetFocus();

    return 0;
}

///----------------------------------------------------------------------------
/// onRButtonDown - Processes the WM_RBUTTONDOWN message. Opens the Tile menu
/// for easy access.
/// @param xPos of the mouse
/// @param yPos of the mouse
/// @return Always 0
///----------------------------------------------------------------------------

LRESULT GameMapPanel::onRButtonDown(const WORD& xPos, const WORD& yPos) {

    if(!gameWorldController->isWorldLoaded()) {
        return 0;
    }

    CPoint viewOffset = GetScrollPosition();

    int row = static_cast<int>((yPos + viewOffset.y) / scaledTileHeight);
    int col = static_cast<int>((xPos + viewOffset.x) / scaledTileWidth);

    if(HIWORD(GetKeyState(VK_LSHIFT)) != 0) {
        
        if(gameWorldController->getGameMap()->isRowColInMapBounds(row, col)) {

            GameTile::DrawInfo di = gameWorldController->getGameMap()->getTile(gameWorldController->getGameMap()->indexFromRowCol(row, col)).getDrawInfo();

            wchar_t debug[256];
            wsprintf(debug, L"Indx: %d\n Modi: %d\n Dark: %d\n", di.spriteIndex, di.spriteModifier, di.dark);
            OutputDebugString(debug);
            return 0;
        }

    }


    // We will try to change to the new tile first, and if that is successful,
    // we will then open up the menu

    if(gameWorldController->trySelectNewTile(row, col)) {
        mainWindow->onGameMapRightClick(xPos, yPos);
    }

    SetFocus();

    return 0;
}

///----------------------------------------------------------------------------
/// onLButtonDBLClick - Processes the WM_LBUTTONDBLCLK message. It will request
/// that the controller changes the selected tile to the one selected in the
/// road palette. If that change happens, it will update the map.
/// @param xPos of the mouse
/// @param yPos of the mouse
/// @return Always 0
///----------------------------------------------------------------------------

LRESULT GameMapPanel::onLButtonDBLClick(const WORD& xPos, const WORD& yPos) {

    if (!gameWorldController->isWorldLoaded()) {
        return 0;
    }

    CPoint viewOffset = GetScrollPosition();

    int row = static_cast<int>((yPos + viewOffset.y) / scaledTileHeight);
    int col = static_cast<int>((xPos + viewOffset.x) / scaledTileWidth);

    // Make sure the user is clicking the map when trying to alter a tile.
    
    // TODO: Make sure it's the same tile twice. If not, forward the call to
    // WM_LBUTTONDOWN

    gameWorldController->tryDrawOnSelectedTile();

    SetFocus();

    return 0;
}

///----------------------------------------------------------------------------
/// updateBackBuffer - Draws the next frame of the back buffer, or it is
/// supposed to.
///----------------------------------------------------------------------------

void GameMapPanel::updateBackBuffer() {

    // Since we need the game map, we'll skip the "isWorldLoaded" call.
    const GameMap* gameMap = gameWorldController->getGameMap();

    CClientDC dc(*this);

    
    if (!gameMap) {

        backBufferBMP = CreateCompatibleBitmap(dc, 1, 1);

        if (backBufferBMP.GetHandle()) {
            CBitmap oldBMP;
            oldBMP = backBufferDC.SelectObject(backBufferBMP);
            backBufferDC.SetPixel(0, 0, RGB(0, 0, 0));
            backBufferDC.SelectObject(oldBMP);
        }
        

        return;
    }

    const int mapWidth  = gameMap->getWidth() * scaledTileWidth;
    const int mapHeight = gameMap->getHeight() * scaledTileHeight;
    
    backBufferBMP = CreateCompatibleBitmap(dc, mapWidth, mapHeight);

    const std::vector<GameTile::DrawInfo> drawDataVec = gameMap->getTileDrawData();

    if (backBufferBMP.GetHandle() && drawDataVec.size() != 0) {

        CBitmap oldBMP;
        oldBMP = backBufferDC.SelectObject(backBufferBMP);

        BLENDFUNCTION fn ={ 0 };
        fn.BlendOp = AC_SRC_OVER;
        fn.SourceConstantAlpha = 192;
        fn.AlphaFormat = 0;

        CMemDC alphaDC(dc);
        alphaDC.CreateCompatibleBitmap(dc, 1, 1);
        alphaDC.SolidFill(RGB(0, 0, 192), CRect(0, 0, 1, 1));

        CMemDC lightOnDC(dc);
        lightOnDC.CreateCompatibleBitmap(dc, 1, 1);
        lightOnDC.SolidFill(RGB(255, 255, 0), CRect(0, 0, 1, 1));

        // TODO: it might be better to return a reference to the entire gamemap
        // object as well   

        const int mapCols = gameMap->getWidth();
        const int mapRows = gameMap->getHeight();

        for (int k = 0; k < mapRows; ++k) {
            for (int i = 0; i < mapCols; ++i) {

                const GameTile::DrawInfo drawInfo = drawDataVec[(k * mapCols) + i];

                const int srcX = drawInfo.spriteIndex * tileWidth;
                const int srcY = (drawInfo.hasGate && gameMap->isConnectedToOnSwitch(k, i)) 
                                 ? TileModifiers::GateOpen * tileHeight 
                                 : drawInfo.spriteModifier * tileHeight;


                const int destX = i * scaledTileWidth;
                const int destY = k * scaledTileHeight;

                backBufferDC.StretchBlt(destX, destY, scaledTileWidth, scaledTileHeight, tilesetDC,
                                        srcX, srcY, tileWidth, tileHeight, SRCCOPY);

                if (drawInfo.dark) {

                    if(gameMap->isConnectedToOnSwitch(k, i)) {
                        AlphaBlend(backBufferDC.GetHDC(), destX, destY,
                                   scaledTileWidth, scaledTileHeight, lightOnDC, 0, 0, 1, 1, fn);
                    }
                    else {
                        AlphaBlend(backBufferDC.GetHDC(), destX, destY,
                                   scaledTileWidth, scaledTileHeight, alphaDC, 0, 0, 1, 1, fn);
                    }
                }

            }
        }

        const int selectedRow = gameWorldController->getSelectedRow();
        const int selectedCol = gameWorldController->getSelectedCol();

        DrawTileSelectionBox(backBufferDC, selectedCol * scaledTileWidth,
                             selectedRow * scaledTileHeight, scaledTileWidth,
                             scaledTileHeight, 2);

        backBufferDC.SelectObject(oldBMP);

    }
     
}


///----------------------------------------------------------------------------
/// updateScrollSize - Updates the size of the scroll bars.
///----------------------------------------------------------------------------

void GameMapPanel::updateScrollSize() {

    const GameMap* gameMap = gameWorldController->getGameMap();

    const int mapWidth  = gameMap ? gameMap->getWidth() * scaledTileWidth : 1;
    const int mapHeight = gameMap ? gameMap->getHeight() * scaledTileHeight : 1;
    CSize newScrollSize(mapWidth, mapHeight);

    SetScrollSizes(newScrollSize);
}