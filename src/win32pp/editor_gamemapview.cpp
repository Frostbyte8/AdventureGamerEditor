#include "editor_mainwindow_views.h"
#include "../editor_constants.h"
#include "shared_functions.h"

//=============================================================================
// Constructors / Destructor
//=============================================================================

GameMapPanel::GameMapPanel(Win32ppMainWindowInterface* inMainWindow, GameWorldController* gwc) :
backBufferDC(NULL), tilesetDC(NULL), mainWindow(inMainWindow), gameWorldController(gwc) {
    fakeZoomLevel = 1;
    tileWidth = 0;
    tileHeight = 0;
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

    int row = static_cast<int>((yPos + viewOffset.y) / tileHeight);
    int col = static_cast<int>((xPos + viewOffset.x) / tileWidth);

    // No need to verify the row and col, the function will do that for us.

    gameWorldController->trySelectNewTile(row, col);

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

    int row = static_cast<int>((yPos + viewOffset.y) / tileHeight);
    int col = static_cast<int>((xPos + viewOffset.x) / tileWidth);

    // We will try to change to the new tile first, and if that is successful,
    // we will then open up the menu

    if(gameWorldController->trySelectNewTile(row, col)) {
        mainWindow->onGameMapRightClick(xPos, yPos);
    }

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

    int row = static_cast<int>((yPos + viewOffset.y) / tileHeight);
    int col = static_cast<int>((xPos + viewOffset.x) / tileWidth);

    // Make sure the user is clicking the map when trying to alter a tile.
    
    // TODO: Make sure it's the same tile twice. If not, forward the call to
    // WM_LBUTTONDOWN

    gameWorldController->tryDrawOnSelectedTile();

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

    const int mapWidth  = gameMap->getWidth() * tileWidth * fakeZoomLevel;
    const int mapHeight = gameMap->getHeight() * tileHeight * fakeZoomLevel;

    
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

        // TODO: it might be better to return a reference to the entire gamemap
        // object as well   

        const int mapCols = gameMap->getWidth();
        const int mapRows = gameMap->getHeight();
        const int width = tileWidth * fakeZoomLevel;
        const int height = tileHeight * fakeZoomLevel;

        for (int k = 0; k < mapRows; ++k) {
            for (int i = 0; i < mapCols; ++i) {

                const GameTile::DrawInfo drawInfo = drawDataVec[(k * mapCols) + i];

                const int srcX = drawInfo.spriteIndex * tileWidth;
                const int srcY = drawInfo.spriteModifier * tileHeight;
                const int destX = i * width;
                const int destY = k * height;

                backBufferDC.StretchBlt(destX, destY, width, height, tilesetDC,
                                        srcX, srcY, tileWidth,
                                        tileHeight, SRCCOPY);

                if (drawInfo.dark) {
                    AlphaBlend(backBufferDC.GetHDC(), destX, destY,
                               width, height, alphaDC, 0, 0, 1, 1, fn);
                }

            }
        }

        const int selectedRow = gameWorldController->getSelectedRow();
        const int selectedCol = gameWorldController->getSelectedCol();

        DrawTileSelectionBox(backBufferDC, selectedCol * tileWidth,
                             selectedRow * tileHeight, tileWidth,
                             tileHeight, 2);

        backBufferDC.SelectObject(oldBMP);

    }
     
}


///----------------------------------------------------------------------------
/// updateScrollSize - Updates the size of the scroll bars.
///----------------------------------------------------------------------------

void GameMapPanel::updateScrollSize() {

    const GameMap* gameMap = gameWorldController->getGameMap();

    const int mapWidth  = gameMap ? gameMap->getWidth() * tileWidth * fakeZoomLevel : 1;
    const int mapHeight = gameMap ? gameMap->getHeight() * tileHeight * fakeZoomLevel : 1;
    CSize newScrollSize(mapWidth, mapHeight);

    SetScrollSizes(newScrollSize);
}