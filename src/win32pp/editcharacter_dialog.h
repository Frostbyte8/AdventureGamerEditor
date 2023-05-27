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

#include "editdialog_base.h"

class EditCharacterDialog : public EditDialogBase {

    public:

        // Constructors
        EditCharacterDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
                            HWND inParentHandle, bool inIsEditCharacter);

        // Accessors
        GameCharacter::Builder getAlteredCharacter();

        // Mutators
        void setCharacterToEdit(const GameCharacter& gameCharacter);

    protected:

        virtual void OnClose();
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual void moveControls();
        virtual bool trySaveData();

        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);        

    private:

        LONG    findLongestTab(const bool getWidth);
        //LRESULT onSize(WPARAM& wParam, LPARAM& lParam);
               
        EditCharacterDescriptionsTab*   descriptionsTab;
        EditCharacterQualitiesTab*      qualitiesTab;
        EditCharacterAttributesTab*     attributesTab;
        EditCharacterMiscTab*           miscTab;
        CButton                         btnDialogControl[3];
        CTab                            tabControl;

        const GameMap*                  gameMap;
        const bool                      isEditCharacter;
        GameCharacter::Builder          newCharacter;
        
};

#endif // __EDITCHARACTER_DIALOG_H__