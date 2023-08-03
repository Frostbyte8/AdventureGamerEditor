#include "editor_mainwindow_views.h"
#include "../editor_constants.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

int RoadSelectorView::OnCreate(CREATESTRUCT& cs) {

    CBrush outofbounds(RGB(0,0,0));
    SetScrollBkgnd(outofbounds);

    return CScrollView::OnCreate(cs);
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// SetTileset - Copies the tileset bitmap into it's own DC for drawing, then
/// makes sure the backbuffer gets updated.
///----------------------------------------------------------------------------

void RoadSelectorView::SetTileset(CBitmap& bmp) {
	
	BITMAP bm;
	bmp.GetObject(sizeof(BITMAP), &bm);
	CMemDC memDC(NULL);
	HBITMAP oldBMP = memDC.SelectObject(bmp);
	tilesetDC.CreateCompatibleBitmap(memDC, bm.bmWidth, bm.bmHeight);
	tilesetDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY);

    tileWidth = bm.bmWidth / EditorConstants::TilesPerCol;
    tileHeight = bm.bmHeight / EditorConstants::TilesPerRow;
	
	memDC.SelectObject(oldBMP);

    UpdateScrollSize();
    UpdateBackBuffer();
    InvalidateRect();
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// SetTileset - Copies the tileset bitmap into it's own DC for drawing, and
/// then updates the backbuffer and redraws itself.
///----------------------------------------------------------------------------

void RoadSelectorView::OnDraw(CDC& dc) {
        
    if(backBufferBMP.GetHandle()) {
		dc.SelectObject(backBufferBMP);        
    }
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// UpdateBackBuffer - Redraws the entire back buffer
///----------------------------------------------------------------------------

void RoadSelectorView::UpdateBackBuffer() {
   
    const int totalHeight = tileHeight * (EditorConstants::TilesPerRow * 2);
    
    // TODO: Verify that the previous BMP is destroyed as per what the docs
    // state.
    
    CClientDC dc(*this);
    backBufferBMP = CreateCompatibleBitmap(dc, tileWidth, totalHeight);
    CBitmap oldBMP;
    oldBMP = backBufferDC.SelectObject(backBufferBMP);


    for(int i = 0; i < EditorConstants::TilesPerRow; ++i) {

        const int yDest = i * tileHeight;
        const int yDirtDest = (i * tileHeight) + (EditorConstants::TilesPerRow * tileHeight);
        const int xSrc = i * tileWidth;
        const int yDirtSrc = EditorConstants::DirtroadStartRow * tileHeight;

        backBufferDC.BitBlt(0, yDest, tileWidth, tileHeight, tilesetDC, xSrc, 0, SRCCOPY);
	    backBufferDC.BitBlt(0, yDirtDest, tileWidth, tileHeight, tilesetDC, xSrc, yDirtSrc, SRCCOPY);

    }

    DrawTileSelectionBox(backBufferDC, 0, 0, tileWidth, tileHeight, 2);

    backBufferDC.SelectObject(oldBMP);
} 

///----------------------------------------------------------------------------
/// UpdateScrollSize - Sets the scroll info for the view based on the current
/// tileset
///----------------------------------------------------------------------------

void RoadSelectorView::UpdateScrollSize() {

    // There are 16 tile types, plus an additional 16 for dirt roads.
    const int totalHeight = tileHeight * (EditorConstants::TilesPerRow * 2);
    CSize scrollSize(tileWidth, totalHeight);
    SetScrollSizes(scrollSize);

}
