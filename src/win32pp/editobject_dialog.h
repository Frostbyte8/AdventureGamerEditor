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

        // Constructors
        EditObjectDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
                            HWND inParentHandle, bool inEditObject);

        // Accessors
        GameObject::Builder getAlteredObject();

        // Mutators
        void setObjectToEdit(const GameObject& gameObject);

    protected:

        virtual void OnClose();
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual void moveControls();
        virtual bool trySaveData();
        
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

    private:

        LONG findLongestTab(const bool getWidth);
        LRESULT onSize(WPARAM& wParam, LPARAM& lParam);

        EditObjectDescriptionsTab*      descriptionsTab;
        EditObjectQualitiesTab*         qualitiesTab;
        EditObjectEffectsTab*           effectsTab;
        EditObjectLocationsTab*         locationsTab;
        CButton                         btnDialogControl[3];
        CTab                            tabControl;

        const GameMap*                  gameMap;
        const bool                      isEditObject;
        GameObject::Builder             newObject;

};

#endif // __EDITOBJECT_DIALOG_H__