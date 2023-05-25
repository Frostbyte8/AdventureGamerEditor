#ifndef __EDITOBJECT_DIALOG_H__
#define __EDITOBJECT_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include <wxx_imagelist.h> // Needed for wxx_tab
#include <wxx_tab.h>

#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../model/gamemap.h"

#include "../interface/mainwindow_interface.h"
#include "editobject_tabviews.h"
#include "editdialog_base.h"

// TODO: Minimum Size and Maximum Size

class EditObjectDialog : public EditDialogBase {

    public:

        EditObjectDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
                            HWND inParentHandle, bool inEditObject);


        GameObject::Builder getAlteredObject();

        void SetObjectToEdit(const GameObject& gameObject) {

            if(descriptionsTab) {

                newObject = GameObject::Builder(gameObject);
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

        bool    okClicked();

        LONG    findLongestTab(const bool getWidth);

        LRESULT OnSize(WPARAM& wParam, LPARAM& lParam);

        const GameMap*                  gameMap;

        GameObject::Builder             newObject;

        EditObjectDescriptionsTab*      descriptionsTab;
        EditObjectQualitiesTab*         qualitiesTab;
        EditObjectEffectsTab*           effectsTab;
        EditObjectLocationsTab*         locationsTab;

        const bool                      isEditObject;
        int                             optionChosen;

        CButton                         btnDialogControl[3];
        CTab                            tabControl;


};



#endif // __EDITOBJECT_DIALOG_H__
