#ifndef __EDITOR_MAINWINDOW_WIN32PP_INTERFACE_H__
#define __EDITOR_MAINWINDOW_WIN32PP_INTERFACE_H__

#include <windows.h>

class Win32ppMainWindowInterface {
    
    public:

        virtual void onGameMapRightClick(const WORD& xPos, const WORD& yPos) = 0;

};

#endif // __EDITOR_MAINWINDOW_WIN32PP_INTERFACE_H__