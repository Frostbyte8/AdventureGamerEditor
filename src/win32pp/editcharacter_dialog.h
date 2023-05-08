#ifndef __EDITCHARACTER_DIALOG_H__
#define __EDITCHARACTER_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include <wxx_imagelist.h> // Needed for wxx_tab
#include <wxx_tab.h>

#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../model/gamemap.h"

#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"
#include "editcharacter_tabviews.h"

class EditCharacterDialog : public CWnd {

    public:

        EditCharacterDialog(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics, const GameMap* inGameMap, bool inEditCharacter);

    protected:

        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

    private:

        MainWindowInterface*            mainWindow;
        const GameMap*                  gameMap;
        WindowMetrics*                  windowMetrics;

};

#endif // __EDITCHARACTER_DIALOG_H__
