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

        void madeChange() {
            changeMade = true;
        }

    protected:

        EditDialogBase(MainWindowInterface* inMainWindow, const GameMap* inGameMap, HWND inParentWindow) : 
                       mainWindow(inMainWindow), gameMap(inGameMap), parentWindow(inParentWindow), changeMade(false) {}

        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, true);
            return true;
        }

        virtual void PreCreate(CREATESTRUCT& cs) {
            cs.style &= ~WS_VISIBLE;
        }
        
        WindowMetrics           windowMetrics;
        MainWindowInterface*    mainWindow;
        const GameMap*          gameMap;
        HWND                    parentWindow;
        bool                    changeMade;

    private:
        


};

#endif // __EDITDIALOG_BASE_H__
