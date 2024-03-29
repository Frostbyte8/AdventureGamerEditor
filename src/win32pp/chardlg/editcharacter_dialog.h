#ifndef __EDITCHARACTER_DIALOG_H__
#define __EDITCHARACTER_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include <wxx_imagelist.h> // Needed for wxx_tab
#include <wxx_tab.h>

#include "../../model/gameobject.h"
#include "../../model/gamecharacter.h"
#include "../../model/gamemap.h"

#include "../../win32/window_metrics.h"
#include "../../interface/mainwindow_interface.h"

#include "editcharacter_tabviews.h"
#include "../editdialog_base.h"

//=============================================================================
// EditCharacterDialog - A Dialog that allows to user to Create a new
// Character, or alter an existing one.
//=============================================================================

class EditCharacterDialog : public EditDialogBase {

    public:

        // Constructors
        EditCharacterDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
                            HWND inParentHandle, bool inIsEditCharacter);

        // Accessors
        GameCharacter::Builder getAlteredCharacter();
        const bool isEditingCharacter() const;

        // Mutators
        void setCharacterToEdit(const GameCharacter& gameCharacter);

    protected:

        // Win32++ Overrides

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

        // Dialog Base Virtual Functions

        virtual void moveControls();
        virtual bool trySaveData();

        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);        

    private:

        LONG    findLongestTab(const bool getWidth);
               
        EditCharacterDescriptionsTab*   descriptionsTab;
        EditCharacterQualitiesTab*      qualitiesTab;
        EditCharacterAttributesTab*     attributesTab;
        EditCharacterMiscTab*           miscTab;
        CTabClassic                     tabControl;

        const GameMap*                  gameMap;
        const bool                      isEditCharacter;
        GameCharacter::Builder          newCharacter;
        
};

#endif // __EDITCHARACTER_DIALOG_H__