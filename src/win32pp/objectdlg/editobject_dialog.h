#ifndef __EDITOBJECT_DIALOG_H__
#define __EDITOBJECT_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include <wxx_imagelist.h> // Needed for wxx_tab
#include <wxx_tab.h>

#include "../../model/gameobject.h"
#include "../../model/gamecharacter.h"
#include "../../model/gamemap.h"

#include "../../interface/mainwindow_interface.h"
#include "editobject_tabviews.h"
#include "../editdialog_base.h"
#include "editobject_interface.h"

// TODO: Minimum Size and Maximum Size

class EditObjectDialog : public EditDialogBase, public EditObjectInterface {

    public:

        // Constructors
        EditObjectDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
                         HWND inParentHandle, bool inEditObject);

        // Accessors
        GameObject::Builder getAlteredObject();
        const bool isEditingObject() const;

        // Mutators
        void setObjectToEdit(const GameObject& gameObject);

        void fixedLocationToggled(const bool& enabled);

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

        virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
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
        CTabClassic                     tabControl;

        const GameMap*                  gameMap;
        const bool                      isEditObject;
        GameObject::Builder             newObject;

};

#endif // __EDITOBJECT_DIALOG_H__
