#ifndef __EDITDIALOG_BASE_H__
#define __EDITDIALOG_BASE_H__

#include <wxx_wincore.h>
#include "../model/gamemap.h"
#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"

class EditDialogBase : public CWnd {
    
    public:

        bool GoModal() {
            if(parentWindow != NULL) {
                ::EnableWindow(parentWindow, FALSE);
                return true;
            }

            return false;
        }

        const CSize& getContentSize() const { return contentSize; }

    protected:

        EditDialogBase(MainWindowInterface* inMainWindow, const GameMap* inGameMap, HWND inParentWindow) : 
                       mainWindow(inMainWindow), gameMap(inGameMap), parentWindow(inParentWindow) {
            contentSize.SetSize(0, 0);
        }

        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, true);
            return true;
        }
        
        CSize                   contentSize;
        WindowMetrics           windowMetrics;
        MainWindowInterface*    mainWindow;
        const GameMap*          gameMap;
        HWND                    parentWindow;

    private:
        


};

#endif // __EDITDIALOG_BASE_H__