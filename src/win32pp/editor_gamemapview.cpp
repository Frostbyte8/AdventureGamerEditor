#include "editor_mainwindow_views.h"
#include "../editor_constants.h"

//=============================================================================
// Constructors / Destructor
//=============================================================================

GameMapView::GameMapView(GameWorldController* gwc) : backBufferDC(NULL), 
                         tilesetDC(NULL), gameWorldController(gwc) {
    fakeZoomLevel = 1;
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// isBMPLoaded - Checks to see if the tileset BMP was loaded successfully.
///----------------------------------------------------------------------------

const bool GameMapView::isBMPLoaded() const {
    return tilesetBMP.GetHandle();
}

//=============================================================================
// Public / Protected functions
//=============================================================================

int GameMapView:: OnCreate(CREATESTRUCT& cs) {

    tilesetBMP.LoadImage(L"tileset.bmp", LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    const int mapWidth = gameWorldController->getMapWidth() * EditorConstants::TileWidth * fakeZoomLevel;
    const int mapHeight = gameWorldController->getMapHeight() * EditorConstants::TileHeight * fakeZoomLevel;

    CSize abc(mapWidth, mapHeight);
    SetScrollSizes(abc); // Otherwise it won't work.
    CBrush outofbounds(RGB(0,0,0));
    SetScrollBkgnd(outofbounds);

    CClientDC dc(*this);
    CBitmap oldBMP;

    tilesetDC = CMemDC(dc);
    tilesetDC.SelectObject(tilesetBMP);

    backBufferBMP = CreateCompatibleBitmap(dc, mapWidth, mapHeight);
    backBufferDC = CMemDC(dc);

    oldBMP = backBufferDC.SelectObject(backBufferBMP);
    backBufferDC.BitBlt(0,0,128,128,tilesetDC,0,0,SRCCOPY);
    backBufferDC.SelectObject(oldBMP);

    return CScrollView::OnCreate(cs);

}

///----------------------------------------------------------------------------
/// OnDraw - 
///----------------------------------------------------------------------------

void GameMapView::OnDraw(CDC& dc) {
    
    const std::vector<GameTile>& gameMap = gameWorldController->getTiles();
    if(backBufferBMP.GetHandle() && gameMap.size() != 0) {

        CBitmap oldBMP;
        oldBMP = backBufferDC.SelectObject(backBufferBMP);

        BLENDFUNCTION fn = {0};
        fn.BlendOp = AC_SRC_OVER;
        fn.SourceConstantAlpha = 192;
        fn.AlphaFormat = 0;

        CMemDC alphaDC(dc);
        alphaDC.CreateCompatibleBitmap(dc, 1, 1);
        alphaDC.SolidFill(RGB(0,0,192), CRect(0,0,1,1));

        // TODO: it might be better to return a reference to the entire gamemap
        // object as well   

        const int mapCols = gameWorldController->getMapWidth();
        const int mapRows = gameWorldController->getMapHeight();
        
        for(int k = 0; k < mapRows; k++) {
            for(int i = 0; i < mapCols; i++) {

                size_t index = (k * mapCols) + i;
                const GameTile gt = gameMap.at(index);

                const int srcX = gt.getSpriteIndex() * EditorConstants::TileWidth;
                const int srcY = gt.getSpriteModifier() * EditorConstants::TileHeight;
                const int destX = i * EditorConstants::TileWidth * fakeZoomLevel;
                const int destY = k * EditorConstants::TileHeight * fakeZoomLevel;
                const int width = EditorConstants::TileWidth * fakeZoomLevel;
                const int height = EditorConstants::TileHeight * fakeZoomLevel;

                backBufferDC.StretchBlt(destX, destY, width, height, tilesetDC, 
                                        srcX, srcY, EditorConstants::TileWidth, EditorConstants::TileHeight, SRCCOPY);

                if(gt.getSpriteModifier() & TileFlags::Dark) {
                    AlphaBlend(backBufferDC.GetHDC(), destX, destY, width, height, alphaDC, 0, 0, 1, 1, fn);
                }
            }
        }
        

        backBufferDC.SelectObject(oldBMP);
        dc.SelectObject(backBufferBMP);

    }
}

///----------------------------------------------------------------------------
/// WndProc
///----------------------------------------------------------------------------

LRESULT GameMapView::WndProc(UINT msg, WPARAM wparam, LPARAM lparam) {
    return WndProcDefault(msg, wparam, lparam);
}