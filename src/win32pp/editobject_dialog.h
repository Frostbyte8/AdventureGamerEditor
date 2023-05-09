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

        EditObjectDialog(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics, const GameMap* inGameMap, bool inEditObject) : 
                         mainWindow(inMainWindow), windowMetrics(inWindowMetrics), gameMap(inGameMap), editObject(inEditObject) {
            contentSize.SetSize(0,0);                             
            parentWindow = 0;
        }

        GameObject::Builder getAlteredObject();

        const CSize& getContentSize() const { return contentSize; }

        void SetParentWindow(const HWND hWnd) {
            parentWindow = hWnd;
        }

        void DoStuff() {
            if(parentWindow != 0) {
                ::EnableWindow(parentWindow, FALSE);
            }
        }

        void SetObjectToEdit(const GameObject& gameObject) {

            if(descriptionsTab) {
                descriptionsTab->populateFields(gameObject, *gameMap);
                qualitiesTab->populateFields(gameObject, *gameMap);
                effectsTab->populateFields(gameObject, *gameMap);
                locationsTab->populateFields(gameObject, *gameMap);
            }
        }

    protected:

        virtual void OnClose(); 
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
        

    private:

        static bool CALLBACK SetProperFont(HWND child, LPARAM font);

        LRESULT OnSize(WPARAM& wParam, LPARAM& lParam);

        MainWindowInterface*            mainWindow;
        const GameMap*                  gameMap;
        WindowMetrics*                  windowMetrics;

        EditObjectDescriptionsTab*      descriptionsTab;
        EditObjectQualitiesTab*         qualitiesTab;
        EditObjectEffectsTab*           effectsTab;
        EditObjectLocationsTab*         locationsTab;

        bool                            editObject;

        CButton                         btnDialogControl[3];
        CTab                            tabControl;
        CSize                           contentSize;

        HWND                            parentWindow;

};



#endif // __EDITOBJECT_DIALOG_H__
