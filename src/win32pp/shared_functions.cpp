#include "shared_functions.h"
#include <wxx_commondlg.h>

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
/// In addition, the border caps at the selection Width even if a bigger
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

///----------------------------------------------------------------------------
/// dlgOnBrowseForMedia - When the Browse for Icon or Sound is pressed on the
/// Edit Character/Object Dialog, opens a Common Dialog box to search for
/// an icon or sound.
/// @param const reference to the dialog making the request
/// @param the edit control that will receive the result.
/// @param if true, it will search for an icon, otherwise, search for a sound.
/// @return true if successful, false if not
///----------------------------------------------------------------------------

bool dlgOnBrowseForMedia(const CWnd& parentDialog, CEdit& txtOut, const bool findIcon) {

    CFileDialog fileDialog(TRUE, NULL, NULL, OFN_NOLONGNAMES | OFN_FILEMUSTEXIST, NULL);

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (findIcon) {

        CString dialogTitle = AtoT(langMap.get("CDBFindImageTitle").c_str(), CP_UTF8);
        CString filterCaption = AtoT(langMap.get("CDBFindImageFilterText").c_str(), CP_UTF8);
        filterCaption += _T(" (*.BMP;*.ICO)|*.BMP;*.ICO|");

        fileDialog.SetFilter(filterCaption);
        fileDialog.SetTitle(dialogTitle);

    } else {

        CString dialogTitle = AtoT(langMap.get("CDBFindSoundTitle").c_str(), CP_UTF8);
        CString filterCaption = AtoT(langMap.get("CDBFindSoundFilterText").c_str(), CP_UTF8);
        filterCaption += _T(" (*.WAV)|*.WAV|");

        fileDialog.SetFilter(filterCaption);
        fileDialog.SetTitle(dialogTitle);

    }

    if (fileDialog.DoModal(parentDialog.GetHwnd()) == IDOK) {

        // Convert the Long Path Name of the file into a short one. We can't use long paths
        // As the game was written for Windows 3.1.

        CString fileName;
        const long strLength = GetShortPathName(fileDialog.GetPathName(), NULL, 0);
        GetShortPathName(fileDialog.GetPathName(), fileName.GetBuffer(strLength), strLength + 1);
        fileName.ReleaseBuffer();

        const int lastSlash = fileName.ReverseFind(_T("\\")) + 1;
        if (lastSlash < 0) {

            CString errText = AtoT(langMap.get("ErrPathConversionText").c_str(), CP_UTF8);
            CString errTitle = AtoT(langMap.get("ErrPathConversionTitle").c_str(), CP_UTF8);

            parentDialog.MessageBox(errText, errTitle, MB_OK | MB_ICONERROR);
            return false;
        }

        fileName = fileName.Mid(lastSlash, fileName.GetLength() - lastSlash);

        txtOut.SetWindowText(fileName);

        return true;

    }

    return false;
}

