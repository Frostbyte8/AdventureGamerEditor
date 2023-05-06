#ifndef __EDITOBJECT_DIALOG_H__
#define __EDITOBJECT_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include <wxx_imagelist.h> // Needed for wxx_tab
#include <wxx_tab.h>

#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../model/gamemap.h"

#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"

#include "editobject_tabviews.h"

// TODO: Window Metrics needs its own instance since if it moves to a differnt monitor
// the metrics may be differnt.

// TODO: Minimum Size and Maximum Size

class EditObjectDialog : public CWnd {

    public:

        EditObjectDialog(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics, const GameMap* inGameMap) : 
                         mainWindow(inMainWindow), windowMetrics(inWindowMetrics), gameMap(inGameMap) {
            //contentSize(32,32);                             
        }

        GameObject::Builder getAlteredObject();

    protected:

        virtual void OnClose(); 
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
        

    private:

        static bool CALLBACK SetFontTest(HWND child, LPARAM font);

        LRESULT OnSize(WPARAM& wParam, LPARAM& lParam);

        MainWindowInterface*            mainWindow;
        const GameMap*                  gameMap;
        WindowMetrics*                  windowMetrics;

        EditObjectDescriptionsTab*      descriptionsTab;
        EditObjectQualitiesTab*         qualitiesTab;
        EditObjectEffectsTab*           effectsTab;
        EditObjectLocationsTab*         locationsTab;

        CButton                         btnDialogControl[3];
        CTab                            tabControl;
        CSize                           contentSize;

};



#endif // __EDITOBJECT_DIALOG_H__
