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

        EditCharacterDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
                            HWND inParentHandle, bool inIsEditCharacter);

        GameCharacter::Builder getAlteredCharacter();

        void SetCharacterToEdit(const GameCharacter& gameCharacter) {

            if(descriptionsTab) {

                newCharacter = GameCharacter::Builder(gameCharacter);
                descriptionsTab->populateFields(gameCharacter, *gameMap);
                qualitiesTab->populateFields(gameCharacter, *gameMap);
                attributesTab->populateFields(gameCharacter, *gameMap);
                miscTab->populateFields(gameCharacter, *gameMap);
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
        bool okClicked();
        
        const GameMap*          gameMap;
        int                             optionChosen;
        const bool                      isEditCharacter;

        GameCharacter::Builder          newCharacter;

        EditCharacterDescriptionsTab*   descriptionsTab;
        EditCharacterQualitiesTab*      qualitiesTab;
        EditCharacterAttributesTab*     attributesTab;
        EditCharacterMiscTab*           miscTab;
        CTab                            tabControl;

        CButton                         btnDialogControl[3];
        
};

#endif // __EDITCHARACTER_DIALOG_H__
