#ifndef __EDITOR_MAINWINDOW_WIN32PP_INTERFACE_H__
#define __EDITOR_MAINWINDOW_WIN32PP_INTERFACE_H__

#include <windows.h>

class Win32ppMainWindowInterface {
    
    public:

        ///--------------------------------------------------------------------
        /// onGameMapRightClick - Inform the window that there was a right
        /// click on the game map, and where it was
        /// @param The x-coordinate of the mouse when it was clicked
        /// @param the y-coordinate of the mouse when it was clicked.
        ///--------------------------------------------------------------------

        virtual void onGameMapRightClick(const WORD& xPos, const WORD& yPos) = 0;

};

#endif // __EDITOR_MAINWINDOW_WIN32PP_INTERFACE_H__