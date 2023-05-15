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

#include "editdialog_base.h"

// TODO: Window Metrics needs its own instance since if it moves to a differnt monitor
// the metrics may be differnt.

// TODO: Minimum Size and Maximum Size

class EditObjectDialog : public EditDialogBase {

    public:

        EditObjectDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
                            HWND inParentHandle, bool inEditObject);


        GameObject::Builder getAlteredObject();

        void SetObjectToEdit(const GameObject& gameObject) {

            // TODO: These need to be set to 0
            if(descriptionsTab) {
                descriptionsTab->populateFields(gameObject, *gameMap);
                qualitiesTab->populateFields(gameObject, *gameMap);
                effectsTab->populateFields(gameObject, *gameMap);
                locationsTab->populateFields(gameObject, *gameMap);
            }
        }

    protected:

        virtual void OnClose(); 
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
        

    private:

        LONG    findLongestTab(const bool getWidth);

        LRESULT OnSize(WPARAM& wParam, LPARAM& lParam);

        GameObject::Builder             newObject;

        EditObjectDescriptionsTab*      descriptionsTab;
        EditObjectQualitiesTab*         qualitiesTab;
        EditObjectEffectsTab*           effectsTab;
        EditObjectLocationsTab*         locationsTab;

        bool                            isEditObject;
        int                             optionChosen;

        CButton                         btnDialogControl[3];
        CTab                            tabControl;


};



#endif // __EDITOBJECT_DIALOG_H__
