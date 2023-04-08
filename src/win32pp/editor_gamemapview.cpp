#include "editor_mainwindow_views.h"
#include "../editor_constants.h"

//=============================================================================
// Constructors / Destructor
//=============================================================================

GameMapView::GameMapView(GameWorldController* gwc) : backBufferDC(NULL), 
                         tilesetDC(NULL), gameWorldController(gwc) {
    fakeZoomLevel = 1;
    tileWidth = 0;
    tileHeight = 0;
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// isBMPLoaded - Checks to see if the tileset BMP was loaded successfully.
///----------------------------------------------------------------------------

const bool GameMapView::isBMPLoaded() const {
    return (tilesetBMP.GetHandle() ? true : false);
}

//=============================================================================
// Public / Protected functions
//=============================================================================

int GameMapView::OnCreate(CREATESTRUCT& cs) {

    tilesetBMP.LoadImage(L"tileset.bmp", LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    double tempWidth     = tilesetBMP.GetSize().cx / EditorConstants::TilesPerCol;
    double tempHeight    = tilesetBMP.GetSize().cy / EditorConstants::TilesPerRow;

    tileWidth   = static_cast<int>(tempWidth);
    tileHeight  = static_cast<int>(tempHeight);

    if(tempWidth - tileWidth != 0) {
        MessageBox(L"Bitmap is not perfectly divisible by 16.", L"", MB_ICONWARNING | MB_OK);
    }

    if(tempHeight - tileHeight != 0) {
        MessageBox(L"Bitmap is not perfectly divisible by 16.", L"", MB_ICONWARNING | MB_OK);
    }
    
    CClientDC dc(*this);
    tilesetDC = CMemDC(dc);
    tilesetDC.SelectObject(tilesetBMP);
    backBufferDC = CMemDC(dc);

    UpdateBackBuffer();

    return CScrollView::OnCreate(cs);

}

void GameMapView::UpdateBackBuffer() {

    const int mapWidth = gameWorldController->getMapWidth() * tileWidth * fakeZoomLevel;
    const int mapHeight = gameWorldController->getMapHeight() * tileHeight * fakeZoomLevel;

    CSize abc(mapWidth, mapHeight);
    SetScrollSizes(abc); // Otherwise it won't work.
    CBrush outofbounds(RGB(0,0,0));
    SetScrollBkgnd(outofbounds);
    
    CBitmap oldBMP;
    CClientDC dc(*this);
    backBufferBMP = CreateCompatibleBitmap(dc, mapWidth, mapHeight);    

    oldBMP = backBufferDC.SelectObject(backBufferBMP);
    backBufferDC.BitBlt(0,0,128,128,tilesetDC,0,0,SRCCOPY);
    backBufferDC.SelectObject(oldBMP);
}

///----------------------------------------------------------------------------
/// OnDraw - 
///----------------------------------------------------------------------------

void GameMapView::OnDraw(CDC& dc) {
        
    const std::vector<GameTile::DrawInfo> drawDataVec = gameWorldController->getTileDrawData();

    if(backBufferBMP.GetHandle() && drawDataVec.size() != 0) {

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
        const int width = tileWidth * fakeZoomLevel;
        const int height = tileHeight * fakeZoomLevel;
       
        for(int k = 0; k < mapRows; ++k) {
            for(int i = 0; i < mapCols; ++i) {

                const GameTile::DrawInfo drawInfo = drawDataVec[(k * mapCols) + i];
            
                const int srcX = drawInfo.spriteIndex * tileWidth;
                const int srcY = drawInfo.spriteModifier * tileHeight;
                const int destX = i * width;
                const int destY = k * height;

                backBufferDC.StretchBlt(destX, destY, width, height, tilesetDC, 
                                        srcX, srcY, tileWidth, tileHeight, SRCCOPY);

                if(drawInfo.dark) {
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
