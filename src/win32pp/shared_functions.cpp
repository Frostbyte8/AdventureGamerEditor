#include "shared_functions.h"

///----------------------------------------------------------------------------
/// centerWindowOnCurrentMonitor - Center's the window on the monitor is is currently on.
/// @param a constant reference to the current monitor
/// @param a reference to the window to be moved
///----------------------------------------------------------------------------

void centerWindowOnCurrentMonitor(const HMONITOR& currentMonitor, CWnd& window) {
    
    RECT rc = window.GetClientRect();
    
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(currentMonitor, &monitorInfo);

    AdjustWindowRectEx(&rc, window.GetStyle(), FALSE, window.GetExStyle());

    // Calculate Where the window is positioned, then offset it to where the monitor is.

    const CPoint windowPos((abs(monitorInfo.rcWork.right - monitorInfo.rcWork.left) / 2) - ((rc.right + abs(rc.left)) / 2) + monitorInfo.rcMonitor.left,
                           (abs(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top) / 2) - ((rc.bottom + abs(rc.top)) / 2) + monitorInfo.rcMonitor.top);

    window.SetWindowPos(0, windowPos.x, windowPos.y, 0, 0, SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_NOREPOSITION);

}

///----------------------------------------------------------------------------
/// DrawTileSelectionBox - Draws a selection box at the coordinates given.
/// @param a reference to a CmemDC to be drawn on
/// @param the x-coordinate to start drawing the box at
/// @param the y-coordinate to start drawing the box at
/// @param the width of the box to be drawn
/// @param the height of the box to be drawn
/// @param species how thick the border should be. It will always have a
/// thickness of 1, and any number smaller than 1 will be rounded up to 1. 
/// In addition, the border caps at the seclection Width even if a bigger
/// thickness is specified.
///----------------------------------------------------------------------------

void DrawTileSelectionBox(CMemDC& inDC, const int& xOffset, const int& yOffset,
                     const int& selectionWidth, const int selectionHeight,
                     const int& borderWidth) {

    CBrush oldBrush = inDC.SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
    CRect selectRect = CRect(xOffset, yOffset, xOffset + selectionWidth, yOffset + selectionHeight);
    inDC.Rectangle(selectRect);
    
    int widthDrawn = 0;
    
    do {
        selectRect.DeflateRect(1, 1);
        inDC.DrawEdge(selectRect, BDR_SUNKENINNER, BF_RECT);   
        widthDrawn++;
    } while(widthDrawn < borderWidth && widthDrawn << selectionWidth);
    
    inDC.SelectObject(oldBrush);

}